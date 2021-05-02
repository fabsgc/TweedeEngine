#include "RenderAPI/TeRenderTexture.h"

#include "Resources/TeResourceManager.h"
#include "Image/TeTextureManager.h"
#include "Image/TePixelUtil.h"
#include "Image/TeTexture.h"

namespace te
{
    RenderTextureProperties::RenderTextureProperties(const RENDER_TEXTURE_DESC& desc, bool requiresFlipping)
    {
        UINT32 firstIdx = (UINT32)-1;
        bool requiresHwGamma = false;
        for (UINT32 i = 0; i < TE_MAX_MULTIPLE_RENDER_TARGETS; i++)
        {
            SPtr<Texture> texture = desc.ColorSurfaces[i].Tex;

            if (texture == nullptr)
                continue;

            if (firstIdx == (UINT32)-1)
                firstIdx = i;

            requiresHwGamma |= texture->GetProperties().IsHardwareGammaEnabled();
        }

        if (firstIdx == (UINT32)-1)
        {
            SPtr<Texture> texture = desc.DepthStencilSurface.Tex;

            if(texture != nullptr)
            {
                const TextureProperties& texProps = texture->GetProperties();
                Construct(&texProps, desc.DepthStencilSurface.NumFaces, desc.DepthStencilSurface.MipLevel,
                            requiresFlipping, false);
            }
        }
        else
        {
            SPtr<Texture> texture = desc.ColorSurfaces[firstIdx].Tex;

            const TextureProperties& texProps = texture->GetProperties();
            Construct(&texProps, desc.ColorSurfaces[firstIdx].NumFaces, desc.ColorSurfaces[firstIdx].MipLevel,
                      requiresFlipping, requiresHwGamma);
        }
    }

    void RenderTextureProperties::Construct(const TextureProperties* textureProps, UINT32 numSlices,
                                            UINT32 mipLevel, bool requiresFlipping, bool hwGamma)
    {
        if (textureProps != nullptr)
        {
            PixelUtil::GetSizeForMipLevel(textureProps->GetWidth(), textureProps->GetHeight(), textureProps->GetDepth(),
                                          mipLevel, Width, Height, numSlices);

            numSlices *= numSlices;
            MultisampleCount = textureProps->GetNumSamples();
        }

        IsWindow = false;
        RequiresTextureFlipping = requiresFlipping;
        this->HWGamma = hwGamma;
    }

    SPtr<RenderTexture> RenderTexture::Create(const TEXTURE_DESC& desc,
        bool createDepth, PixelFormat depthStencilFormat)
    {
        return TextureManager::Instance().CreateRenderTexture(desc, createDepth, depthStencilFormat);
    }

    SPtr<RenderTexture> RenderTexture::Create(const RENDER_TEXTURE_DESC& desc)
    {
        return TextureManager::Instance().CreateRenderTexture(desc);
    }

    RenderTexture::RenderTexture(const RENDER_TEXTURE_DESC& desc, UINT32 deviceIdx)
        : _desc(desc)
    { }

    void RenderTexture::Initialize()
    {
        RenderTarget::Initialize();

        for (UINT32 i = 0; i < TE_MAX_MULTIPLE_RENDER_TARGETS; i++)
        {
            if (_desc.ColorSurfaces[i].Tex != nullptr)
            {
                SPtr<Texture> texture = _desc.ColorSurfaces[i].Tex;

                if ((texture->GetProperties().GetUsage() & TU_RENDERTARGET) == 0)
                    TE_ASSERT_ERROR(false, "Provided texture is not created with render target usage.");

                _colorSurfaces[i] = texture->RequestView(_desc.ColorSurfaces[i].MipLevel, 1,
                    _desc.ColorSurfaces[i].Face, _desc.ColorSurfaces[i].NumFaces, GVU_RENDERTARGET);
            }
        }

        if (_desc.DepthStencilSurface.Tex != nullptr)
        {
            SPtr<Texture> texture = _desc.DepthStencilSurface.Tex;

            if ((texture->GetProperties().GetUsage() & TU_DEPTHSTENCIL) == 0)
            {
                TE_ASSERT_ERROR(false, "Provided texture is not created with depth stencil usage.");
            }

            _depthStencilSurface = texture->RequestView(_desc.DepthStencilSurface.MipLevel, 1,
                _desc.DepthStencilSurface.Face, _desc.DepthStencilSurface.NumFaces, GVU_DEPTHSTENCIL);
        }

        ThrowIfBuffersDontMatch();
    }

    void RenderTexture::ThrowIfBuffersDontMatch() const
    {
        UINT32 firstSurfaceIdx = (UINT32)-1;
        for (UINT32 i = 0; i < TE_MAX_MULTIPLE_RENDER_TARGETS; i++)
        {
            if (_colorSurfaces[i] == nullptr)
                continue;

            if (firstSurfaceIdx == (UINT32)-1)
            {
                firstSurfaceIdx = i;
                continue;
            }

            const TextureProperties& curTexProps = _desc.ColorSurfaces[i].Tex->GetProperties();
            const TextureProperties& firstTexProps = _desc.ColorSurfaces[firstSurfaceIdx].Tex->GetProperties();

            UINT32 curMsCount = curTexProps.GetNumSamples();
            UINT32 firstMsCount = firstTexProps.GetNumSamples();

            UINT32 curNumSlices = _colorSurfaces[i]->GetNumArraySlices();
            UINT32 firstNumSlices = _colorSurfaces[firstSurfaceIdx]->GetNumArraySlices();

            if (curMsCount == 0)
                curMsCount = 1;

            if (firstMsCount == 0)
                firstMsCount = 1;

            if (curTexProps.GetWidth() != firstTexProps.GetWidth() ||
                curTexProps.GetHeight() != firstTexProps.GetHeight() ||
                curTexProps.GetDepth() != firstTexProps.GetDepth() ||
                curMsCount != firstMsCount ||
                curNumSlices != firstNumSlices)
            {
                String errorInfo = "\nWidth: " + ToString(curTexProps.GetWidth()) + "/" + ToString(firstTexProps.GetWidth());
                errorInfo += "\nHeight: " + ToString(curTexProps.GetHeight()) + "/" + ToString(firstTexProps.GetHeight());
                errorInfo += "\nDepth: " + ToString(curTexProps.GetDepth()) + "/" + ToString(firstTexProps.GetDepth());
                errorInfo += "\nNum. slices: " + ToString(curNumSlices) + "/" + ToString(firstNumSlices);
                errorInfo += "\nMultisample Count: " + ToString(curMsCount) + "/" + ToString(firstMsCount);

                TE_ASSERT_ERROR(false, "Provided color textures don't match!" + errorInfo);
            }
        }

        if (firstSurfaceIdx != (UINT32)-1)
        {
            const TextureProperties& firstTexProps = _desc.ColorSurfaces[firstSurfaceIdx].Tex->GetProperties();
            SPtr<TextureView> firstSurfaceView = _colorSurfaces[firstSurfaceIdx];

            UINT32 numSlices;
            if (firstTexProps.GetTextureType() == TEX_TYPE_3D)
                numSlices = firstTexProps.GetDepth();
            else
                numSlices = firstTexProps.GetNumFaces();

            if ((firstSurfaceView->GetFirstArraySlice() + firstSurfaceView->GetNumArraySlices()) > numSlices)
            {
                TE_ASSERT_ERROR(false, "Provided number of faces is out of range. Face: " +
                    ToString(firstSurfaceView->GetFirstArraySlice() + firstSurfaceView->GetNumArraySlices()) + ". Max num faces: " + ToString(numSlices));
            }

            if (firstSurfaceView->GetMostDetailedMip() > firstTexProps.GetNumMipmaps())
            {
                TE_ASSERT_ERROR(false, "Provided number of mip maps is out of range. Mip level: " +
                    ToString(firstSurfaceView->GetMostDetailedMip()) + ". Max num mipmaps: " + ToString(firstTexProps.GetNumMipmaps()));
            }

            if (_depthStencilSurface == nullptr)
                return;

            const TextureProperties& depthTexProps = _desc.DepthStencilSurface.Tex->GetProperties();
            UINT32 depthMsCount = depthTexProps.GetNumSamples();
            UINT32 colorMsCount = firstTexProps.GetNumSamples();

            if (depthMsCount == 0)
                depthMsCount = 1;

            if (colorMsCount == 0)
                colorMsCount = 1;

            if (depthTexProps.GetWidth() != firstTexProps.GetWidth() ||
                depthTexProps.GetHeight() != firstTexProps.GetHeight() ||
                depthMsCount != colorMsCount)
            {
                String errorInfo = "\nWidth: " + ToString(depthTexProps.GetWidth()) + "/" + ToString(firstTexProps.GetWidth());
                errorInfo += "\nHeight: " + ToString(depthTexProps.GetHeight()) + "/" + ToString(firstTexProps.GetHeight());
                errorInfo += "\nMultisample Count: " + ToString(depthMsCount) + "/" + ToString(colorMsCount);

                TE_ASSERT_ERROR(false, "Provided texture and depth stencil buffer don't match!" + errorInfo);
            }
        }
    }
}
