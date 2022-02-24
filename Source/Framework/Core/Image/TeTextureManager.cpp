#include "TeTextureManager.h"
#include "Image/TeColor.h"
#include "TePixelData.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(TextureManager)

    void TextureManager::OnStartUp()
    {
        TEXTURE_DESC desc;
        desc.Type = TEX_TYPE_2D;
        desc.Width = 2;
        desc.Height = 2;
        desc.Format = PF_RGBA8;
        desc.Usage = TU_STATIC;

        // White built-in texture
        desc.DebugName = "White Texture";
        SPtr<Texture> whiteTexture = CreateTexture(desc);

        SPtr<PixelData> whitePixelData = PixelData::Create(2, 2, 1, PF_RGBA8);
        whitePixelData->SetColorAt(Color::White, 0, 0);
        whitePixelData->SetColorAt(Color::White, 0, 1);
        whitePixelData->SetColorAt(Color::White, 1, 0);
        whitePixelData->SetColorAt(Color::White, 1, 1);

        whiteTexture->WriteData(*whitePixelData);
        Texture::WHITE = whiteTexture;

        // Black built-in texture
        desc.DebugName = "Black Texture";
        SPtr<Texture> blackTexture = CreateTexture(desc);

        SPtr<PixelData> blackPixelData = PixelData::Create(2, 2, 1, PF_RGBA8);
        blackPixelData->SetColorAt(Color::ZERO, 0, 0);
        blackPixelData->SetColorAt(Color::ZERO, 0, 1);
        blackPixelData->SetColorAt(Color::ZERO, 1, 0);
        blackPixelData->SetColorAt(Color::ZERO, 1, 1);

        blackTexture->WriteData(*blackPixelData);
        Texture::BLACK = blackTexture;

        // Normal (Y = Up) built-in texture
        desc.DebugName = "Normal Texture";
        SPtr<Texture> normalTexture = CreateTexture(desc);
        SPtr<PixelData> normalPixelData = PixelData::Create(2, 2, 1, PF_RGBA8);

        Color encodedNormal(0.5f, 0.5f, 1.0f);
        normalPixelData->SetColorAt(encodedNormal, 0, 0);
        normalPixelData->SetColorAt(encodedNormal, 0, 1);
        normalPixelData->SetColorAt(encodedNormal, 1, 0);
        normalPixelData->SetColorAt(encodedNormal, 1, 1);

        normalTexture->WriteData(*normalPixelData);
        Texture::NORMAL = normalTexture;
    }

    void TextureManager::OnShutDown()
    {
        // Need to make sure these are freed while still on the core thread
        Texture::WHITE = nullptr;
        Texture::BLACK = nullptr;
        Texture::NORMAL = nullptr;
    }

    SPtr<Texture> TextureManager::CreateTexture(const TEXTURE_DESC& desc)
    {
        SPtr<Texture> newTex = CreateTextureInternal(desc, nullptr);
        newTex->Initialize();

        return newTex;
    }

    SPtr<Texture> TextureManager::CreateTexture(const TEXTURE_DESC& desc, const SPtr<PixelData>& pixelData)
    {
        SPtr<Texture> newTex = CreateTextureInternal(desc, pixelData);
        newTex->Initialize();

        return newTex;
    }

    SPtr<RenderTexture> TextureManager::CreateRenderTexture(const TEXTURE_DESC& colorDesc, bool createDepth, 
        PixelFormat depthStencilFormat)
    {
        TEXTURE_DESC textureDesc = colorDesc;
        textureDesc.Usage = TU_RENDERTARGET;
        textureDesc.NumMips = 0;

        HTexture texture = Texture::Create(textureDesc);

        HTexture depthStencil;
        if (createDepth)
        {
            textureDesc.Format = depthStencilFormat;
            textureDesc.HwGamma = false;
            textureDesc.Usage = TU_DEPTHSTENCIL;

            depthStencil = Texture::Create(textureDesc);
        }

        RENDER_TEXTURE_DESC desc;
        desc.ColorSurfaces[0].Tex = texture.GetInternalPtr();
        desc.ColorSurfaces[0].Face = 0;
        desc.ColorSurfaces[0].NumFaces = 1;
        desc.ColorSurfaces[0].MipLevel = 0;

        desc.DepthStencilSurface.Tex = depthStencil.GetInternalPtr();
        desc.DepthStencilSurface.Face = 0;
        desc.DepthStencilSurface.NumFaces = 1;
        desc.DepthStencilSurface.MipLevel = 0;

        SPtr<RenderTexture> newRT = CreateRenderTexture(desc);

        return newRT;
    }

    SPtr<RenderTexture> TextureManager::CreateRenderTexture(const RENDER_TEXTURE_DESC& desc)
    {
        SPtr<RenderTexture> newRT = CreateRenderTextureInternal(desc);
        newRT->SetThisPtr(newRT);
        newRT->Initialize();

        return newRT;
    }
}
