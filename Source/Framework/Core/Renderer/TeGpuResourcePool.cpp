#include "TeGpuResourcePool.h"
#include "RenderAPI/TeRenderTexture.h"
#include "Image/TeTexture.h"
#include "RenderAPI/TeGpuBuffer.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(GpuResourcePool)

    SPtr<PooledRenderTexture> GpuResourcePool::Get(const POOLED_RENDER_TEXTURE_DESC& desc)
    {
        for (auto& entry : _textures)
        {
            bool isFree = entry.use_count() == 1;
            if (!isFree)
                continue;

            if (entry->Tex == nullptr)
                continue;

            if (Matches(entry->Tex, desc))
            {
                entry->_lastUsedFrame = _currentFrame;
                return entry;
            }
        }

        TE_PRINT("new resource")

        SPtr<PooledRenderTexture> newTexture = te_shared_ptr_new<PooledRenderTexture>(_currentFrame);
        _textures.push_back(newTexture);

        TEXTURE_DESC texDesc;
        texDesc.Type = desc.Type;
        texDesc.Width = desc.Width;
        texDesc.Height = desc.Height;
        texDesc.Depth = desc.Depth;
        texDesc.Format = desc.Format;
        texDesc.Usage = desc.Flag;
        texDesc.HwGamma = desc.HwGamma;
        texDesc.NumSamples = desc.NumSamples;
        texDesc.NumMips = desc.NumMipLevels;

        if (desc.Type != TEX_TYPE_3D)
            texDesc.NumArraySlices = desc.ArraySize;

        newTexture->Tex = Texture::_createPtr(texDesc);

        if ((desc.Flag & (TU_RENDERTARGET | TU_DEPTHSTENCIL)) != 0)
        {
            RENDER_TEXTURE_DESC rtDesc;

            if ((desc.Flag & TU_RENDERTARGET) != 0)
            {
                rtDesc.ColorSurfaces[0].Tex = newTexture->Tex;
                rtDesc.ColorSurfaces[0].Face = 0;
                rtDesc.ColorSurfaces[0].NumFaces = newTexture->Tex->GetProperties().GetNumFaces();
                rtDesc.ColorSurfaces[0].MipLevel = 0;
            }

            if ((desc.Flag & TU_DEPTHSTENCIL) != 0)
            {
                rtDesc.DepthStencilSurface.Tex = newTexture->Tex;
                rtDesc.DepthStencilSurface.Face = 0;
                rtDesc.DepthStencilSurface.NumFaces = newTexture->Tex->GetProperties().GetNumFaces();
                rtDesc.DepthStencilSurface.MipLevel = 0;
            }

            newTexture->RenderTex = RenderTexture::Create(rtDesc);
        }

        return newTexture;
    }

    void GpuResourcePool::Get(SPtr<PooledRenderTexture> & texture, const POOLED_RENDER_TEXTURE_DESC & desc)
    {
        if (texture && Matches(texture->Tex, desc))
            return;

        texture = Get(desc);
    }

    SPtr<PooledStorageBuffer> GpuResourcePool::Get(const POOLED_STORAGE_BUFFER_DESC& desc)
    { 
        for (auto& entry : _buffers)
        {
            bool isFree = entry.use_count() == 1;
            if (!isFree)
                continue;

            if (entry->Buffer == nullptr)
                continue;

            if (Matches(entry->Buffer, desc))
            {
                entry->_lastUsedFrame = _currentFrame;
                return entry;
            }
        }

        SPtr<PooledStorageBuffer> newBuffer = te_shared_ptr_new<PooledStorageBuffer>(_currentFrame);
        _buffers.push_back(newBuffer);

        GPU_BUFFER_DESC bufferDesc;
        bufferDesc.Type = desc.Type;
        bufferDesc.ElementSize = desc.ElementSize;
        bufferDesc.ElementCount = desc.NumElements;
        bufferDesc.Format = desc.Format;
        bufferDesc.Usage = desc.Usage;

        newBuffer->Buffer = GpuBuffer::Create(bufferDesc);

        return newBuffer;
    }

    void GpuResourcePool::Get(SPtr<PooledStorageBuffer>& buffer, const POOLED_STORAGE_BUFFER_DESC& desc)
    {
        if (buffer && Matches(buffer->Buffer, desc))
            return;

        buffer = Get(desc);
    }

    void GpuResourcePool::Update()
    {
        _currentFrame++;

        // Note: Should also force pruning when over some memory limit (in which case I can probably increase the
        // age pruning limit higher)
        Prune(3);
    }

    void GpuResourcePool::Prune(UINT32 age)
    {
        for (auto iter = _textures.begin(); iter != _textures.end();)
        {
            auto& entry = *iter;

            bool isFree = entry.use_count() == 1;
            if (!isFree)
            {
                ++iter;
                continue;
            }

            UINT32 entryAge = _currentFrame - entry->_lastUsedFrame;
            if (entryAge >= age)
                iter = _textures.erase(iter);
            else
                ++iter;
        }

        for (auto iter = _buffers.begin(); iter != _buffers.end();)
        {
            auto& entry = *iter;

            bool isFree = entry.use_count() == 1;
            if (!isFree)
            {
                ++iter;
                continue;
            }

            UINT32 entryAge = _currentFrame - entry->_lastUsedFrame;
            if (entryAge >= age)
                iter = _buffers.erase(iter);
            else
                ++iter;
        }
    }

    bool GpuResourcePool::Matches(const SPtr<Texture>& texture, const POOLED_RENDER_TEXTURE_DESC& desc)
    {
        const TextureProperties& texProps = texture->GetProperties();

        bool match = texProps.GetTextureType() == desc.Type
            && texProps.GetFormat() == desc.Format
            && texProps.GetWidth() == desc.Width
            && texProps.GetHeight() == desc.Height
            && (texProps.GetUsage() & desc.Flag) == desc.Flag
            && (
            (desc.Type == TEX_TYPE_2D
                && texProps.IsHardwareGammaEnabled() == desc.HwGamma
                && texProps.GetNumSamples() == desc.NumSamples)
                || (desc.Type == TEX_TYPE_3D
                    && texProps.GetDepth() == desc.Depth)
                || (desc.Type == TEX_TYPE_CUBE_MAP)
                )
            && texProps.GetNumArraySlices() == desc.ArraySize
            && texProps.GetNumMipmaps() == desc.NumMipLevels
            ;

        return match;
    }

    bool GpuResourcePool::Matches(const SPtr<GpuBuffer>& buffer, const POOLED_STORAGE_BUFFER_DESC& desc)
    {
        const GpuBufferProperties& props = buffer->GetProperties();

        bool match = props.GetType() == desc.Type && props.GetElementCount() == desc.NumElements;
        if (match)
        {
            if (desc.Type == GBT_STANDARD)
                match = props.GetFormat() == desc.Format;
            else // Structured
                match = props.GetElementSize() == desc.ElementSize;

            if (match)
                match = props.GetUsage() == desc.Usage;
        }

        return match;
    }

    POOLED_RENDER_TEXTURE_DESC POOLED_RENDER_TEXTURE_DESC::Create2D(PixelFormat format, UINT32 width, UINT32 height,
        INT32 usage, UINT32 samples, bool hwGamma, UINT32 arraySize, UINT32 mipCount)
    {
        POOLED_RENDER_TEXTURE_DESC desc;
        desc.Width = width;
        desc.Height = height;
        desc.Depth = 1;
        desc.Format = format;
        desc.NumSamples = samples;
        desc.Flag = (TextureUsage)usage;
        desc.HwGamma = hwGamma;
        desc.Type = TEX_TYPE_2D;
        desc.ArraySize = arraySize;
        desc.NumMipLevels = mipCount;

        return desc;
    }

    POOLED_RENDER_TEXTURE_DESC POOLED_RENDER_TEXTURE_DESC::Create3D(PixelFormat format, UINT32 width, UINT32 height,
        UINT32 depth, INT32 usage)
    {
        POOLED_RENDER_TEXTURE_DESC desc;
        desc.Width = width;
        desc.Height = height;
        desc.Depth = depth;
        desc.Format = format;
        desc.NumSamples = 1;
        desc.Flag = (TextureUsage)usage;
        desc.HwGamma = false;
        desc.Type = TEX_TYPE_3D;
        desc.ArraySize = 1;
        desc.NumMipLevels = 0;

        return desc;
    }

    POOLED_RENDER_TEXTURE_DESC POOLED_RENDER_TEXTURE_DESC::CreateCube(PixelFormat format, UINT32 width, UINT32 height,
        INT32 usage, UINT32 arraySize)
    {
        POOLED_RENDER_TEXTURE_DESC desc;
        desc.Width = width;
        desc.Height = height;
        desc.Depth = 1;
        desc.Format = format;
        desc.NumSamples = 1;
        desc.Flag = (TextureUsage)usage;
        desc.HwGamma = false;
        desc.Type = TEX_TYPE_CUBE_MAP;
        desc.ArraySize = arraySize;
        desc.NumMipLevels = 0;

        return desc;
    }

    POOLED_STORAGE_BUFFER_DESC POOLED_STORAGE_BUFFER_DESC::CreateStandard(GpuBufferFormat format, UINT32 numElements,
        GpuBufferUsage usage)
    {
        POOLED_STORAGE_BUFFER_DESC desc;
        desc.Type = GBT_STANDARD;
        desc.Format = format;
        desc.NumElements = numElements;
        desc.ElementSize = 0;
        desc.Usage = usage;

        return desc;
    }

    POOLED_STORAGE_BUFFER_DESC POOLED_STORAGE_BUFFER_DESC::CreateStructured(UINT32 elementSize, UINT32 numElements,
        GpuBufferUsage usage)
    {
        POOLED_STORAGE_BUFFER_DESC desc;
        desc.Type = GBT_STRUCTURED;
        desc.Format = BF_UNKNOWN;
        desc.NumElements = numElements;
        desc.ElementSize = elementSize;
        desc.Usage = usage;

        return desc;
    }

    GpuResourcePool& gGpuResourcePool()
    {
        return GpuResourcePool::Instance();
    }
}
