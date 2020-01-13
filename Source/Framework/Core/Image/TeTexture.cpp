#include "Image/TeTexture.h"
#include "Math/TeMath.h"
#include "Resources/TeResourceManager.h"
#include "Image/TeTextureManager.h"
#include "Image/TePixelUtil.h"

namespace te
{
    TextureProperties::TextureProperties(const TEXTURE_DESC& desc)
        : _desc(desc)
    { }

    bool TextureProperties::HasAlpha() const
    {
        return PixelUtil::HasAlpha(_desc.Format);
    }

    UINT32 TextureProperties::GetNumFaces() const
    {
        UINT32 facesPerSlice = GetTextureType() == TEX_TYPE_CUBE_MAP ? 6 : 1;
        return facesPerSlice * _desc.NumArraySlices;
    }

    void TextureProperties::MapFromSubresourceIdx(UINT32 subresourceIdx, UINT32& face, UINT32& mip) const
    {
        UINT32 numMipmaps = GetNumMipmaps() + 1;

        face = Math::FloorToInt((subresourceIdx) / (float)numMipmaps);
        mip = subresourceIdx % numMipmaps;
    }

    UINT32 TextureProperties::MapToSubresourceIdx(UINT32 face, UINT32 mip) const
    {
        return face * (GetNumMipmaps() + 1) + mip;
    }

    SPtr<PixelData> TextureProperties::AllocBuffer(UINT32 face, UINT32 mipLevel) const
    {
        UINT32 width = GetWidth();
        UINT32 height = GetHeight();
        UINT32 depth = GetDepth();

        for (UINT32 j = 0; j < mipLevel; j++)
        {
            if (width != 1) width /= 2;
            if (height != 1) height /= 2;
            if (depth != 1) depth /= 2;
        }

        SPtr<PixelData> dst = te_shared_ptr_new<PixelData>(width, height, depth, GetFormat());
        dst->AllocateInternalBuffer();

        return dst;
    }

    SPtr<Texture> Texture::WHITE;
    SPtr<Texture> Texture::BLACK;
    SPtr<Texture> Texture::NORMAL;

    Texture::Texture(const TEXTURE_DESC& desc)
        : _properties(desc)
    { }

    Texture::Texture(const TEXTURE_DESC& desc, const SPtr<PixelData>& pixelData)
        : _properties(desc)
        , _initData(pixelData)
    { }

    void Texture::Initialize()
    {
        _size = CalculateSize();
        
        Resource::Initialize();
        CoreObject::Initialize();
    }

    void Texture::Copy(const SPtr<Texture>& target, const TEXTURE_COPY_DESC& desc)
    {
        // TODO
    }

    void Texture::Clear(const Color& value, UINT32 mipLevel, UINT32 face, UINT32 queueIdx)
    {
        // TODO
    }

    void Texture::ReadData(PixelData& dest, UINT32 mipLevel , UINT32 face, UINT32 deviceIdx, UINT32 queueIdx)
    {
        // TODO
    }

    void Texture::WriteData(const PixelData& src, UINT32 mipLevel, UINT32 face, bool discardWholeBuffer, UINT32 queueIdx)
    {
        // TODO
    }

    UINT32 Texture::CalculateSize() const
    {
        return _properties.GetNumFaces() * PixelUtil::GetMemorySize(_properties.GetWidth(),
               _properties.GetHeight(), _properties.GetDepth(), _properties.GetFormat());
    }

    HTexture Texture::Create(const TEXTURE_DESC& desc)
    {
        SPtr<Texture> texturePtr = _createPtr(desc);

        return static_resource_cast<Texture>(gResourceManager()._createResourceHandle(texturePtr));
    }

    HTexture Texture::Create(const SPtr<PixelData>& pixelData, int usage, bool hwGammaCorrection)
    {
        SPtr<Texture> texturePtr = _createPtr(pixelData, usage, hwGammaCorrection);

        return static_resource_cast<Texture>(gResourceManager()._createResourceHandle(texturePtr));
    }

    SPtr<Texture> Texture::_createPtr(const TEXTURE_DESC& desc)
    {
        return TextureManager::Instance().CreateTexture(desc);
    }

    SPtr<Texture> Texture::_createPtr(const SPtr<PixelData>& pixelData, int usage, bool hwGammaCorrection)
    {
        TEXTURE_DESC desc;
        desc.Type = pixelData->GetDepth() > 1 ? TEX_TYPE_3D : TEX_TYPE_2D;
        desc.Width = pixelData->GetWidth();
        desc.Height = pixelData->GetHeight();
        desc.Depth = pixelData->GetDepth();
        desc.Format = pixelData->GetFormat();
        desc.Usage = usage;
        desc.HwGamma = hwGammaCorrection;

        return TextureManager::Instance().CreateTexture(desc, pixelData);
    }
}
