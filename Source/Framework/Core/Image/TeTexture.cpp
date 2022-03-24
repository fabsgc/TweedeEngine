#include "Image/TeTexture.h"
#include "Math/TeMath.h"
#include "Resources/TeResourceManager.h"
#include "Image/TeTextureManager.h"
#include "Image/TePixelUtil.h"
#include "Utility/TeBitwise.h"

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

        if (!dst->GetData())
        {
            TE_ASSERT_ERROR(dst->GetData(), "Failed to allocate a buffer");
        }

        return dst;
    }

    SPtr<Texture> Texture::WHITE;
    SPtr<Texture> Texture::BLACK;
    SPtr<Texture> Texture::NORMAL;

    Texture::Texture()
        : Resource(TID_Texture)
    { }

    Texture::Texture(const TEXTURE_DESC& desc)
        : Resource(TID_Texture)
        , _properties(desc)
    { }

    Texture::Texture(const TEXTURE_DESC& desc, const SPtr<PixelData>& pixelData)
        : Resource(TID_Texture)
        , _properties(desc)
        , _initData(pixelData)
    { }

    void Texture::Initialize()
    {
        _size = CalculateSize();

        // Allocate CPU buffers if needed
        if ((_properties.GetUsage() & TU_CPUCACHED) != 0)
        {
            CreateCPUBuffers();

            if (_initData != nullptr)
            {
                UpdateCPUBuffers(0, *_initData);
            }
        }

        Resource::Initialize();
    }

    PixelData Texture::Lock(GpuLockOptions options, UINT32 mipLevel, UINT32 face, UINT32 deviceIdx, UINT32 queueIdx)
    {
        if (mipLevel > _properties.GetNumMipmaps())
        {
            TE_DEBUG("Invalid mip level: " + ToString(mipLevel) + ". Min is 0, max is " + ToString(_properties.GetNumMipmaps()));
            return PixelData(0, 0, 0, PF_UNKNOWN);
        }

        if (face >= _properties.GetNumFaces())
        {
            TE_DEBUG("Invalid face index: " + ToString(face) + ". Min is 0, max is " + ToString(_properties.GetNumFaces()));
            return PixelData(0, 0, 0, PF_UNKNOWN);
        }

        return LockImpl(options, mipLevel, face, deviceIdx, queueIdx);
    }

    void Texture::Unlock()
    {
        UnlockImpl();
    }

    void Texture::Copy(const SPtr<Texture>& target, const TEXTURE_COPY_DESC& desc)
    {
        if (target->_properties.GetTextureType() != _properties.GetTextureType())
        {
            TE_DEBUG("Source and destination textures must be of same type.");
            return;
        }

        if (_properties.GetFormat() != target->_properties.GetFormat()) // Note: It might be okay to use different formats of the same size
        {
            TE_DEBUG("Source and destination texture formats must match.");
            return;
        }

        if (target->_properties.GetNumSamples() > 1 && _properties.GetNumSamples() != target->_properties.GetNumSamples())
        {
            TE_DEBUG("When copying to a multisampled texture, source texture must have the same number of samples.");
            return;
        }

        if (desc.SrcFace >= _properties.GetNumFaces())
        {
            TE_DEBUG("Invalid source face index.");
            return;
        }

        if (desc.DstFace >= target->_properties.GetNumFaces())
        {
            TE_DEBUG("Invalid destination face index.");
            return;
        }

        if (desc.SrcMip > _properties.GetNumMipmaps())
        {
            TE_DEBUG("Source mip level out of range. Valid range is [0, " + ToString(_properties.GetNumMipmaps()) + "].");
            return;
        }

        if (desc.DstMip > target->_properties.GetNumMipmaps())
        {
            TE_DEBUG("Destination mip level out of range. Valid range is [0, " + ToString(target->_properties.GetNumMipmaps()) + "].");
            return;
        }

        UINT32 srcWidth, srcHeight, srcDepth;
        PixelUtil::GetSizeForMipLevel(
            _properties.GetWidth(),
            _properties.GetHeight(),
            _properties.GetDepth(),
            desc.SrcMip,
            srcWidth,
            srcHeight,
            srcDepth);

        UINT32 dstWidth, dstHeight, dstDepth;
        PixelUtil::GetSizeForMipLevel(
            target->_properties.GetWidth(),
            target->_properties.GetHeight(),
            target->_properties.GetDepth(),
            desc.DstMip,
            dstWidth,
            dstHeight,
            dstDepth);

        if (desc.DstPosition.x < 0 || desc.DstPosition.x >= (INT32)dstWidth ||
            desc.DstPosition.y < 0 || desc.DstPosition.y >= (INT32)dstHeight ||
            desc.DstPosition.z < 0 || desc.DstPosition.z >= (INT32)dstDepth)
        {
            TE_DEBUG("Destination position falls outside the destination texture.");
            return;
        }

        bool entireSurface = desc.SrcVolume.GetWidth() == 0 ||
            desc.SrcVolume.GetHeight() == 0 ||
            desc.SrcVolume.GetDepth() == 0;

        UINT32 dstRight = (UINT32)desc.DstPosition.x;
        UINT32 dstBottom = (UINT32)desc.DstPosition.y;
        UINT32 dstBack = (UINT32)desc.DstPosition.z;
        if (!entireSurface)
        {
            if (desc.SrcVolume.Left >= srcWidth || desc.SrcVolume.Right > srcWidth ||
                desc.SrcVolume.Top >= srcHeight || desc.SrcVolume.Bottom > srcHeight ||
                desc.SrcVolume.Front >= srcDepth || desc.SrcVolume.Back > srcDepth)
            {
                TE_DEBUG("Source volume falls outside the source texture.");
                return;
            }

            dstRight += desc.SrcVolume.GetWidth();
            dstBottom += desc.SrcVolume.GetHeight();
            dstBack += desc.SrcVolume.GetDepth();
        }
        else
        {
            dstRight += srcWidth;
            dstBottom += srcHeight;
            dstBack += srcDepth;
        }

        if (dstRight > dstWidth || dstBottom > dstHeight || dstBack > dstDepth)
        {
            TE_DEBUG("Destination volume falls outside the destination texture.");
            return;
        }

        CopyImpl(target, desc);
    }

    void Texture::Clear(const Color& value, UINT32 mipLevel, UINT32 face, UINT32 queueIdx)
    {
        if (face >= _properties.GetNumFaces())
        {
            TE_DEBUG("Invalid face index.");
            return;
        }

        if (mipLevel > _properties.GetNumMipmaps())
        {
            TE_DEBUG("Mip level out of range. Valid range is [0, " + ToString(_properties.GetNumMipmaps()) + "].");
            return;
        }

        ClearImpl(value, mipLevel, face, queueIdx);
    }

    void Texture::ClearImpl(const Color& value, UINT32 mipLevel, UINT32 face, UINT32 queueIdx)
    {
        SPtr<PixelData> data = _properties.AllocBuffer(face, mipLevel);
        data->SetColors(value);

        WriteData(*data, mipLevel, face, true, queueIdx);
    }

    void Texture::ReadData(PixelData& dest, UINT32 mipLevel , UINT32 face, UINT32 deviceIdx, UINT32 queueIdx)
    {
        PixelData& pixelData = static_cast<PixelData&>(dest);

        UINT32 mipWidth, mipHeight, mipDepth;
        PixelUtil::GetSizeForMipLevel(_properties.GetWidth(), _properties.GetHeight(), _properties.GetDepth(),
            mipLevel, mipWidth, mipHeight, mipDepth);

        if (pixelData.GetWidth() != mipWidth || pixelData.GetHeight() != mipHeight ||
            pixelData.GetDepth() != mipDepth || pixelData.GetFormat() != _properties.GetFormat())
        {
            TE_DEBUG("Provided buffer is not of valid dimensions or format in order to read from this texture.");
            return;
        }

        ReadDataImpl(pixelData, mipLevel, face, deviceIdx, queueIdx);
    }

    void Texture::ReadCachedData(PixelData& dest, UINT32 face, UINT32 mipLevel)
    {
        if ((_properties.GetUsage() & TU_CPUCACHED) == 0)
        {
            TE_DEBUG("Attempting to read CPU data from a texture that is created without CPU caching.");
            return;
        }

        UINT32 mipWidth, mipHeight, mipDepth;
        PixelUtil::GetSizeForMipLevel(_properties.GetWidth(), _properties.GetHeight(), _properties.GetDepth(),
            mipLevel, mipWidth, mipHeight, mipDepth);

        if (dest.GetWidth() != mipWidth || dest.GetHeight() != mipHeight ||
            dest.GetDepth() != mipDepth || dest.GetFormat() != _properties.GetFormat())
        {
            TE_DEBUG("Provided buffer is not of valid dimensions or format in order to read from this texture.");
            return;
        }

        UINT32 subresourceIdx = _properties.MapToSubresourceIdx(face, mipLevel);
        if (subresourceIdx >= (UINT32)_CPUSubresourceData.size())
        {
            TE_DEBUG("Invalid subresource index: " + ToString(subresourceIdx) + ". Supported range: 0 .. " + ToString(_CPUSubresourceData.size() - 1));
            return;
        }

        if (_CPUSubresourceData[subresourceIdx]->GetSize() != dest.GetSize())
        {
            TE_ASSERT_ERROR(false, "Buffer sizes don't match.");
        }

        UINT8* srcPtr = _CPUSubresourceData[subresourceIdx]->GetData();
        UINT8* destPtr = dest.GetData();

        memcpy(destPtr, srcPtr, dest.GetSize());
    }

    void Texture::WriteData(const PixelData& src, UINT32 mipLevel, UINT32 face, bool discardWholeBuffer, UINT32 queueIdx)
    {
        UINT32 subresourceIdx = _properties.MapToSubresourceIdx(face, mipLevel);
        UpdateCPUBuffers(subresourceIdx, src);

        if (discardWholeBuffer)
        {
            if ((_properties.GetUsage() & TU_DYNAMIC) == 0)
            {
                // Buffer discard is enabled but buffer was not created as dynamic. Disabling discard.
                TE_DEBUG("Buffer discard is enabled but buffer was not created as dynamic. Disabling discard.");
                discardWholeBuffer = false;
            }
        }

        WriteDataImpl(src, mipLevel, face, discardWholeBuffer, queueIdx);
    }

    UINT32 Texture::CalculateSize() const
    {
        return _properties.GetNumFaces() * PixelUtil::GetMemorySize(_properties.GetWidth(),
               _properties.GetHeight(), _properties.GetDepth(), _properties.GetFormat());
    }

    HTexture Texture::Create(const TEXTURE_DESC& desc)
    {
        SPtr<Texture> texturePtr = CreatePtr(desc);

        return static_resource_cast<Texture>(gResourceManager()._createResourceHandle(texturePtr));
    }

    HTexture Texture::Create(const SPtr<PixelData>& pixelData, int usage, bool hwGammaCorrection)
    {
        SPtr<Texture> texturePtr = CreatePtr(pixelData, usage, hwGammaCorrection);

        return static_resource_cast<Texture>(gResourceManager()._createResourceHandle(texturePtr));
    }

    SPtr<Texture> Texture::CreatePtr(const TEXTURE_DESC& desc)
    {
        return TextureManager::Instance().CreateTexture(desc);
    }

    SPtr<Texture> Texture::CreatePtr(const SPtr<PixelData>& pixelData, int usage, bool hwGammaCorrection)
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

    SPtr<TextureView> Texture::CreateView(const TEXTURE_VIEW_DESC& desc)
    {
        return te_shared_ptr<TextureView>(new (te_allocate<TextureView>()) TextureView(desc));
    }

    void Texture::ClearBufferViews()
    {
        _textureViews.clear();
    }

    void Texture::CreateCPUBuffers()
    {
        UINT32 numFaces = _properties.GetNumFaces();
        UINT32 numMips = _properties.GetNumMipmaps() + 1;

        UINT32 numSubresources = numFaces * numMips;
        _CPUSubresourceData.resize(numSubresources);

        for (UINT32 i = 0; i < numFaces; i++)
        {
            UINT32 curWidth = _properties.GetWidth();
            UINT32 curHeight = _properties.GetHeight();
            UINT32 curDepth = _properties.GetDepth();

            for (UINT32 j = 0; j < numMips; j++)
            {
                UINT32 subresourceIdx = _properties.MapToSubresourceIdx(i, j);

                _CPUSubresourceData[subresourceIdx] = te_shared_ptr_new<PixelData>(curWidth, curHeight, curDepth, _properties.GetFormat());
                _CPUSubresourceData[subresourceIdx]->AllocateInternalBuffer();

                if (curWidth > 1)
                    curWidth = curWidth / 2;

                if (curHeight > 1)
                    curHeight = curHeight / 2;

                if (curDepth > 1)
                    curDepth = curDepth / 2;
            }
        }
    }

    void Texture::UpdateCPUBuffers(UINT32 subresourceIdx, const PixelData& pixelData)
    {
        if ((_properties.GetUsage() & TU_CPUCACHED) == 0)
        {
            return;
        }

        if (subresourceIdx >= (UINT32)_CPUSubresourceData.size())
        {
            TE_DEBUG("Invalid subresource index: " + ToString(subresourceIdx) + ". Supported range: 0 .. " + ToString(_CPUSubresourceData.size() - 1));
            return;
        }

        UINT32 mipLevel;
        UINT32 face;
        _properties.MapFromSubresourceIdx(subresourceIdx, face, mipLevel);

        UINT32 mipWidth, mipHeight, mipDepth;
        PixelUtil::GetSizeForMipLevel(_properties.GetWidth(), _properties.GetHeight(), _properties.GetDepth(),
            mipLevel, mipWidth, mipHeight, mipDepth);

        if (pixelData.GetWidth() != mipWidth || pixelData.GetHeight() != mipHeight ||
            pixelData.GetDepth() != mipDepth || pixelData.GetFormat() != _properties.GetFormat())
        {
            TE_DEBUG("Provided buffer is not of valid dimensions or format in order to update this texture.");
            return;
        }

        if (_CPUSubresourceData[subresourceIdx]->GetSize() != pixelData.GetSize())
        {
            TE_ASSERT_ERROR(false, "Buffer sizes don't match.");
        }

        UINT8* dest = _CPUSubresourceData[subresourceIdx]->GetData();
        UINT8* src = pixelData.GetData();

        memcpy(dest, src, pixelData.GetSize());
    }

    SPtr<TextureView> Texture::RequestView(UINT32 mostDetailMip, UINT32 numMips, UINT32 firstArraySlice, UINT32 numArraySlices, 
        GpuViewUsage usage, const String& debugName)
    {
        const TextureProperties& texProps = GetProperties();

        TEXTURE_VIEW_DESC key;
        key.MostDetailMip = mostDetailMip;
        key.NumMips = numMips == 0 ? (texProps.GetNumMipmaps() + 1) : numMips;
        key.FirstArraySlice = firstArraySlice;
        key.NumArraySlices = numArraySlices == 0 ? texProps.GetNumFaces() : numArraySlices;
        key.Usage = usage;
        key.DebugName = debugName;

        auto iterFind = _textureViews.find(key);
        if (iterFind == _textureViews.end())
        {
            _textureViews[key] = CreateView(key);

            iterFind = _textureViews.find(key);
        }

        return iterFind->second;
    }
}
