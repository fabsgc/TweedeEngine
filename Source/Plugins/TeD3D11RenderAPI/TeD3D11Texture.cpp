#include "TeD3D11Texture.h"
#include "TeD3D11RenderAPI.h"
#include "TeD3D11Device.h"
#include "TeD3D11Mappings.h"
#include "TeD3D11TextureView.h"
#include "TeD3D11Utility.h"
#include "Image/TePixelUtil.h"
#include "Profiling/TeProfilerGPU.h"

namespace te
{
    D3D11Texture::D3D11Texture(const TEXTURE_DESC& desc, const SPtr<PixelData>& initialData)
        : Texture(desc, initialData)
    { }

    D3D11Texture::~D3D11Texture()
    {
        ClearBufferViews();

        SAFE_RELEASE(_tex);
        SAFE_RELEASE(_1DTex);
        SAFE_RELEASE(_2DTex);
        SAFE_RELEASE(_3DTex);
        SAFE_RELEASE(_stagingBuffer);

        TE_INC_PROFILER_GPU(ResDestroyed);
    }

    void D3D11Texture::Initialize()
    {
        switch (_properties.GetTextureType())
        {
        case TEX_TYPE_1D:
            Create1DTex();
            break;
        case TEX_TYPE_2D:
        case TEX_TYPE_CUBE_MAP:
            Create2DTex();
            break;
        case TEX_TYPE_3D:
            Create3DTex();
            break;
        default:
            TE_ASSERT_ERROR(false, "Unknown texture type");
        }

        TE_INC_PROFILER_GPU(ResCreated);
        Texture::Initialize();
    }

    ID3D11ShaderResourceView* D3D11Texture::GetSRV() const
    {
        return _shaderResourceView->GetSRV();
    }

    PixelData D3D11Texture::LockImpl(GpuLockOptions options, UINT32 mipLevel, UINT32 face, UINT32 deviceIdx, UINT32 queueIdx)
    {
        if (_properties.GetNumSamples() > 1)
        {
            TE_ASSERT_ERROR(false, "Multisampled textures cannot be accessed from the CPU directly.");
        }

#if TE_PROFILING_ENABLED
        if (options == GBL_READ_ONLY || options == GBL_READ_WRITE)
        {
            TE_INC_PROFILER_GPU(ResRead);
        }

        if (options == GBL_READ_WRITE || options == GBL_WRITE_ONLY || options == GBL_WRITE_ONLY_DISCARD || options == GBL_WRITE_ONLY_NO_OVERWRITE)
        {
            TE_INC_PROFILER_GPU(ResWrite);
        }
#endif

        UINT32 mipWidth = std::max(1u, _properties.GetWidth() >> mipLevel);
        UINT32 mipHeight = std::max(1u, _properties.GetHeight() >> mipLevel);
        UINT32 mipDepth = std::max(1u, _properties.GetDepth() >> mipLevel);

        PixelData lockedArea(mipWidth, mipHeight, mipDepth, _internalFormat);

        D3D11_MAP flags = D3D11Mappings::GetLockOptions(options);
        UINT32 rowPitch, slicePitch;
        if (flags == D3D11_MAP_READ || flags == D3D11_MAP_READ_WRITE)
        {
            UINT8* data = (UINT8*)Mapstagingbuffer(flags, face, mipLevel, rowPitch, slicePitch);
            lockedArea.SetExternalBuffer(data);
            lockedArea.SetRowPitch(rowPitch);
            lockedArea.SetSlicePitch(slicePitch);

            _lockedForReading = true;
        }
        else
        {
            if ((_properties.GetUsage() & TU_DYNAMIC) != 0)
            {
                if (flags == D3D11_MAP_WRITE)
                {
                    TE_DEBUG("Dynamic textures only support discard or no-overwrite writes. Falling back to no-overwrite.");
                    flags = D3D11_MAP_WRITE_DISCARD;
                }

                UINT8* data = (UINT8*)Map(_tex, flags, face, mipLevel, rowPitch, slicePitch);
                lockedArea.SetExternalBuffer(data);
                lockedArea.SetRowPitch(rowPitch);
                lockedArea.SetSlicePitch(slicePitch);
            }
            else
                lockedArea.SetExternalBuffer((UINT8*)Mapstaticbuffer(lockedArea, mipLevel, face));

            _lockedForReading = false;
        }

        return lockedArea;
    }

    void D3D11Texture::UnlockImpl()
    {
        if (_lockedForReading)
        {
            Unmapstagingbuffer();
        }
        else
        {
            if ((_properties.GetUsage() & TU_DYNAMIC) != 0)
            {
                Unmap(_tex);
            }
            else
            {
                Unmapstaticbuffer();
            }
        }
    }

    void D3D11Texture::CopyImpl(const SPtr<Texture>& target, const TEXTURE_COPY_DESC& desc)
    {
        D3D11Texture* other = static_cast<D3D11Texture*>(target.get());

        UINT32 srcResIdx = D3D11CalcSubresource(desc.SrcMip, desc.SrcFace, _properties.GetNumMipmaps() + 1);
        UINT32 destResIdx = D3D11CalcSubresource(desc.DstMip, desc.DstFace, target->GetProperties().GetNumMipmaps() + 1);

        D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::InstancePtr());
        D3D11Device& device = rs->GetPrimaryDevice();

        bool srcHasMultisample = _properties.GetNumSamples() > 1;
        bool destHasMultisample = target->GetProperties().GetNumSamples() > 1;

        bool copyEntireSurface = desc.SrcVolume.GetWidth() == 0 ||
            desc.SrcVolume.GetHeight() == 0 ||
            desc.SrcVolume.GetDepth() == 0;

        {
            device.LockContext();

            if (srcHasMultisample && !destHasMultisample) // Resolving from MS to non-MS texture
            {
                if(copyEntireSurface)
                    device.GetImmediateContext()->ResolveSubresource(other->GetDX11Resource(), destResIdx, _tex, srcResIdx, _DXGIFormat);
                else
                {
                    // Need to first resolve to a temporary texture, then copy
                    TEXTURE_DESC tempDesc;
                    tempDesc.Width = _properties.GetWidth();
                    tempDesc.Height = _properties.GetHeight();
                    tempDesc.Format = _properties.GetFormat();
                    tempDesc.HwGamma = _properties.IsHardwareGammaEnabled();
                    tempDesc.DebugName = _properties.GetDebugName();

                    SPtr<D3D11Texture> temporary = std::static_pointer_cast<D3D11Texture>(Texture::CreatePtr(tempDesc));
                    device.GetImmediateContext()->ResolveSubresource(temporary->GetDX11Resource(), 0, _tex, srcResIdx, _DXGIFormat);

                    TEXTURE_COPY_DESC tempCopyDesc;
                    tempCopyDesc.DstMip = desc.DstMip;
                    tempCopyDesc.DstFace = desc.DstFace;
                    tempCopyDesc.DstPosition = desc.DstPosition;

                    temporary->Copy(target, tempCopyDesc);
                }
            }
            else
            {
                D3D11_BOX srcRegion;
                srcRegion.left = desc.SrcVolume.Left;
                srcRegion.right = desc.SrcVolume.Right;
                srcRegion.top = desc.SrcVolume.Top;
                srcRegion.bottom = desc.SrcVolume.Bottom;
                srcRegion.front = desc.SrcVolume.Front;
                srcRegion.back = desc.SrcVolume.Back;

                D3D11_BOX* srcRegionPtr = nullptr;
                if(!copyEntireSurface)
                    srcRegionPtr = &srcRegion;

                device.GetImmediateContext()->CopySubresourceRegion(
                    other->GetDX11Resource(),
                    destResIdx,
                    (UINT32)desc.DstPosition.x,
                    (UINT32)desc.DstPosition.y,
                    (UINT32)desc.DstPosition.z,
                    _tex,
                    srcResIdx,
                    srcRegionPtr);

                if (device.HasError())
                {
                    String errorDescription = device.GetErrorDescription();
                    TE_ASSERT_ERROR(false, "D3D11 device cannot copy subresource\nError Description: " + errorDescription);
                }
            }

            device.UnlockContext();
        }
    }

    void D3D11Texture::ReadDataImpl(PixelData& dest, UINT32 mipLevel, UINT32 face, UINT32 deviceIdx, UINT32 queueIdx)
    {
        if (_properties.GetNumSamples() > 1)
        {
            TE_DEBUG("Multisampled textures cannot be accessed from the CPU directly.");
            return;
        }

        PixelData myData = Lock(GBL_READ_ONLY, mipLevel, face, deviceIdx, queueIdx);
        PixelUtil::BulkPixelConversion(myData, dest);
        Unlock();
    }

    void D3D11Texture::WriteDataImpl(const PixelData& src, UINT32 mipLevel, UINT32 face, bool discardWholeBuffer, UINT32 queueIdx)
    {
        PixelFormat format = _properties.GetFormat();

        if (_properties.GetNumSamples() > 1)
        {
            TE_DEBUG("Multisampled textures cannot be accessed from the CPU directly.");
            return;
        }

        mipLevel = Math::Clamp(mipLevel, (UINT32)mipLevel, _properties.GetNumMipmaps());
        face = Math::Clamp(face, (UINT32)0, _properties.GetNumFaces() - 1);

        if (face > 0 && _properties.GetTextureType() == TEX_TYPE_3D)
        {
            TE_DEBUG("3D texture arrays are not supported.");
            return;
        }

        if ((_properties.GetUsage() & TU_DYNAMIC) != 0)
        {
            PixelData myData = Lock(discardWholeBuffer ? GBL_WRITE_ONLY_DISCARD : GBL_WRITE_ONLY, mipLevel, face, 0, queueIdx);
            PixelUtil::BulkPixelConversion(src, myData);
            Unlock();
        }
        else if ((_properties.GetUsage() & TU_DEPTHSTENCIL) == 0)
        {
            D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::InstancePtr());
            D3D11Device& device = rs->GetPrimaryDevice();

            UINT subresourceIdx = D3D11CalcSubresource(mipLevel, face, _properties.GetNumMipmaps() + 1);
            UINT32 rowWidth = D3D11Mappings::GetSizeInBytes(format, src.GetWidth());
            UINT32 sliceWidth = D3D11Mappings::GetSizeInBytes(format, src.GetWidth(), src.GetHeight());

            {
                device.LockContext();
                device.GetImmediateContext()->UpdateSubresource(_tex, subresourceIdx, nullptr, src.GetData(), rowWidth, sliceWidth);
                device.UnlockContext();

                if (device.HasError())
                {
                    String errorDescription = device.GetErrorDescription();
                    TE_ASSERT_ERROR(false, "D3D11 device cannot map texture\nError Description: " + errorDescription);
                }

                TE_INC_PROFILER_GPU(ResWrite);
            }
        }
        else
        {
            TE_ASSERT_ERROR(false, "Trying to write into a buffer with unsupported usage: " + ToString(_properties.GetUsage()));
        }
    }

    void D3D11Texture::Create1DTex()
    {
        UINT32 width = _properties.GetWidth();
        int usage = _properties.GetUsage();
        UINT32 numMips = _properties.GetNumMipmaps();
        PixelFormat format = _properties.GetFormat();
        bool hwGamma = _properties.IsHardwareGammaEnabled();
        PixelFormat closestFormat = D3D11Mappings::GetClosestSupportedPF(format, TEX_TYPE_1D, usage);
        UINT32 numFaces = _properties.GetNumFaces();
        const String& debugName = "[TEX] " + _properties.GetDebugName();

        // We must have those defined here
        assert(width > 0);

        // Determine which D3D11 pixel format we'll use
        HRESULT hr;
        DXGI_FORMAT d3dPF = D3D11Mappings::GetPF(closestFormat, hwGamma);

        if (format != closestFormat)
            TE_DEBUG("Provided pixel format is not supported by the driver");

        _internalFormat = closestFormat;
        _DXGIColorFormat = d3dPF;
        _DXGIDepthStencilFormat = d3dPF;

        // TODO - Consider making this a parameter eventually
        bool readableDepth = true;

        D3D11_TEXTURE1D_DESC desc;
        desc.Width = static_cast<UINT32>(width);
        desc.ArraySize = numFaces == 0 ? 1 : numFaces;
        desc.Format = d3dPF;
        desc.MiscFlags = 0;

        if ((usage & TU_RENDERTARGET) != 0)
        {
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
            desc.CPUAccessFlags = 0;
            desc.MipLevels = 1;
        }
        else if ((usage & TU_DEPTHSTENCIL) != 0)
        {
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

            desc.CPUAccessFlags = 0;
            desc.MipLevels = 1;
            desc.Format = D3D11Mappings::GetTypelessDepthStencilPF(closestFormat);

            _DXGIColorFormat = D3D11Mappings::GetShaderResourceDepthStencilPF(closestFormat);
            _DXGIDepthStencilFormat = d3dPF;
        }
        else
        {
            desc.Usage = D3D11Mappings::GetUsage((GpuBufferUsage)usage);
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            desc.CPUAccessFlags = D3D11Mappings::GetAccessFlags((GpuBufferUsage)usage);

            // Determine total number of mipmaps including main one (d3d11 convention)
            desc.MipLevels = (numMips == MIP_UNLIMITED || (1U << numMips) > width) ? 0 : numMips + 1;
        }

        if ((usage & TU_LOADSTORE) != 0)
            desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;

        // Create the texture
        D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::InstancePtr());
        D3D11Device& device = rs->GetPrimaryDevice();
        hr = device.GetD3D11Device()->CreateTexture1D(&desc, nullptr, &_1DTex);

        // Check result and except if failed
        if (FAILED(hr) || device.HasError())
        {
            String errorDescription = device.GetErrorDescription();
            TE_ASSERT_ERROR(false, "Error creating texture\nError Description: " + errorDescription);
        }

        hr = _1DTex->QueryInterface(__uuidof(ID3D11Resource), (void**)&_tex);

        if (FAILED(hr) || device.HasError())
        {
            String errorDescription = device.GetErrorDescription();
            TE_ASSERT_ERROR(false, "Can't get base texture\nError Description: " + errorDescription);
        }

        _1DTex->GetDesc(&desc);

        if (numMips != (desc.MipLevels - 1))
            TE_ASSERT_ERROR(false, "Driver returned different number of mip maps than requested. Requested: " + ToString(numMips) + ". Got: " + ToString(desc.MipLevels - 1) + ".");

        _DXGIFormat = desc.Format;

        if ((usage & TU_DEPTHSTENCIL) == 0 || readableDepth)
        {
            TEXTURE_VIEW_DESC viewDesc;
            viewDesc.MostDetailMip = 0;
            viewDesc.NumMips = desc.MipLevels;
            viewDesc.FirstArraySlice = 0;
            viewDesc.NumArraySlices = desc.ArraySize;
            viewDesc.Usage = GVU_DEFAULT;
            viewDesc.DebugName = debugName;

            _shaderResourceView = te_shared_ptr<D3D11TextureView>(new (te_allocate<D3D11TextureView>()) D3D11TextureView(this, viewDesc));
        }

        D3D11Utility::SetDebugName(_2DTex, debugName.c_str(), debugName.size());
    }

    void D3D11Texture::Create2DTex()
    {
        UINT32 width = _properties.GetWidth();
        UINT32 height = _properties.GetHeight();
        int usage = _properties.GetUsage();
        UINT32 numMips = _properties.GetNumMipmaps();
        PixelFormat format = _properties.GetFormat();
        bool hwGamma = _properties.IsHardwareGammaEnabled();
        UINT32 sampleCount = _properties.GetNumSamples();
        TextureType texType = _properties.GetTextureType();
        PixelFormat closestFormat = D3D11Mappings::GetClosestSupportedPF(format, texType, usage);
        UINT32 numFaces = _properties.GetNumFaces();
        const String& debugName = "[TEX] " + _properties.GetDebugName();

        // TODO - Consider making this a parameter eventually
        bool readableDepth = true;
        
        // 0-sized textures aren't supported by the API
        width = std::max(width, 1U);
        height = std::max(height, 1U);

        // Determine which D3D11 pixel format we'll use
        HRESULT hr;
        DXGI_FORMAT d3dPF = D3D11Mappings::GetPF(closestFormat, hwGamma);

        _internalFormat = closestFormat;
        _DXGIColorFormat = d3dPF;
        _DXGIDepthStencilFormat = d3dPF;

        D3D11_TEXTURE2D_DESC desc;
        desc.Width = static_cast<UINT32>(width);
        desc.Height = static_cast<UINT32>(height);
        desc.ArraySize = numFaces == 0 ? 1 : numFaces;;
        desc.Format = d3dPF;
        desc.MiscFlags = 0;

        if ((usage & TU_RENDERTARGET) != 0)
        {
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // TODO - Add flags to allow RT be created without shader resource flags (might be more optimal)
            desc.CPUAccessFlags = 0;

            DXGI_SAMPLE_DESC sampleDesc;
            D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::InstancePtr());
            rs->DetermineMultisampleSettings(sampleCount, d3dPF, &sampleDesc);
            desc.SampleDesc = sampleDesc;
        }
        else if ((usage & TU_DEPTHSTENCIL) != 0)
        {
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.CPUAccessFlags = 0;
            desc.Format = D3D11Mappings::GetTypelessDepthStencilPF(closestFormat);
            desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

            DXGI_SAMPLE_DESC sampleDesc;
            D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::InstancePtr());
            rs->DetermineMultisampleSettings(sampleCount, d3dPF, &sampleDesc);
            desc.SampleDesc = sampleDesc;

            _DXGIColorFormat = D3D11Mappings::GetShaderResourceDepthStencilPF(closestFormat);
            _DXGIDepthStencilFormat = d3dPF;
        }
        else
        {
            desc.Usage = D3D11Mappings::GetUsage((GpuBufferUsage)usage);
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            desc.CPUAccessFlags = D3D11Mappings::GetAccessFlags((GpuBufferUsage)usage);

            DXGI_SAMPLE_DESC sampleDesc;
            sampleDesc.Count = 1;
            sampleDesc.Quality = 0;
            desc.SampleDesc = sampleDesc;
        }

        // Determine total number of mipmaps including main one (d3d11 convention)
        //desc.MipLevels = (numMips == MIP_UNLIMITED || (1U << numMips) > width) ? 0 : numMips + 1;
        desc.MipLevels = (numMips == MIP_UNLIMITED) ? 0 : numMips + 1;

        if (texType == TEX_TYPE_CUBE_MAP)
            desc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;

        if ((usage & TU_LOADSTORE) != 0)
        {
            if (desc.SampleDesc.Count <= 1)
                desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
            else
                TE_DEBUG("Unable to create a load-store texture with multiple samples. This is not supported on DirectX 11. Ignoring load-store usage flag.");
        }

        // Create the texture
        D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::InstancePtr());
        D3D11Device& device = rs->GetPrimaryDevice();
        hr = device.GetD3D11Device()->CreateTexture2D(&desc, nullptr, &_2DTex);

        // Check result and except if failed
        if (FAILED(hr) || device.HasError())
        {
            String errorDescription = device.GetErrorDescription();
            TE_ASSERT_ERROR(false, "Error creating texture\nError Description: " + errorDescription);
        }

        hr = _2DTex->QueryInterface(__uuidof(ID3D11Resource), (void**)&_tex);

        if (FAILED(hr) || device.HasError())
        {
            String errorDescription = device.GetErrorDescription();
            TE_ASSERT_ERROR(false, "Can't get base texture\nError Description: " + errorDescription);
        }

        _2DTex->GetDesc(&desc);

        if (numMips != (desc.MipLevels - 1))
        {
            TE_ASSERT_ERROR(false, "Driver returned different number of mip maps than requested. Requested: " + ToString(numMips) + ". Got: " + ToString(desc.MipLevels - 1) + ".");
        }

        _DXGIFormat = desc.Format;

        // Create shader texture view
        if ((usage & TU_DEPTHSTENCIL) == 0 || readableDepth)
        {
            TEXTURE_VIEW_DESC viewDesc;
            viewDesc.MostDetailMip = 0;
            viewDesc.NumMips = desc.MipLevels;
            viewDesc.FirstArraySlice = 0;
            viewDesc.NumArraySlices = desc.ArraySize;
            viewDesc.Usage = GVU_DEFAULT;
            viewDesc.DebugName = debugName;

            _shaderResourceView = te_shared_ptr<D3D11TextureView>(new (te_allocate<D3D11TextureView>()) D3D11TextureView(this, viewDesc));
        }

        D3D11Utility::SetDebugName(_2DTex, debugName.c_str(), debugName.size());
    }

    void D3D11Texture::Create3DTex()
    {
        UINT32 width = _properties.GetWidth();
        UINT32 height = _properties.GetHeight();
        UINT32 depth = _properties.GetDepth();
        int usage = _properties.GetUsage();
        UINT32 numMips = _properties.GetNumMipmaps();
        PixelFormat format = _properties.GetFormat();
        bool hwGamma = _properties.IsHardwareGammaEnabled();
        PixelFormat closestFormat = D3D11Mappings::GetClosestSupportedPF(format, TEX_TYPE_3D, usage);
        const String& debugName = "[TEX] " + _properties.GetDebugName();

        // TODO - Consider making this a parameter eventually
        bool readableDepth = true;

        // We must have those defined here
        assert(width > 0 && height > 0 && depth > 0);

        // Determine which D3D11 pixel format we'll use
        HRESULT hr;
        DXGI_FORMAT d3dPF = D3D11Mappings::GetPF(closestFormat, hwGamma);

        _internalFormat = closestFormat;
        _DXGIColorFormat = d3dPF;
        _DXGIDepthStencilFormat = d3dPF;

        D3D11_TEXTURE3D_DESC desc;
        desc.Width = static_cast<UINT32>(width);
        desc.Height = static_cast<UINT32>(height);
        desc.Depth = static_cast<UINT32>(depth);
        desc.Format = d3dPF;
        desc.MiscFlags = 0;

        if ((_properties.GetUsage() & TU_RENDERTARGET) != 0)
        {
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
            desc.CPUAccessFlags = 0;
            desc.MipLevels = 1;
        }
        else if ((_properties.GetUsage() & TU_DEPTHSTENCIL) != 0)
        {
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.CPUAccessFlags = 0;
            desc.MipLevels = 1;
            desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

            _DXGIColorFormat = D3D11Mappings::GetShaderResourceDepthStencilPF(closestFormat);
            _DXGIDepthStencilFormat = d3dPF;
        }
        else
        {
            desc.Usage = D3D11Mappings::GetUsage((GpuBufferUsage)usage);
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            desc.CPUAccessFlags = D3D11Mappings::GetAccessFlags((GpuBufferUsage)usage);

            // Determine total number of mipmaps including main one (d3d11 convention)
            desc.MipLevels = (numMips == MIP_UNLIMITED || (1U << numMips)
                > std::max(std::max(width, height), depth)) ? 0 : numMips + 1;
        }

        if ((usage & TU_LOADSTORE) != 0)
            desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;

        // Create the texture
        D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::InstancePtr());
        D3D11Device& device = rs->GetPrimaryDevice();
        hr = device.GetD3D11Device()->CreateTexture3D(&desc, nullptr, &_3DTex);

        // Check result and except if failed
        if (FAILED(hr) || device.HasError())
        {
            String errorDescription = device.GetErrorDescription();
            TE_ASSERT_ERROR(false, "Error creating texture\nError Description: " + errorDescription);
        }

        hr = _3DTex->QueryInterface(__uuidof(ID3D11Resource), (void**)&_tex);

        if (FAILED(hr) || device.HasError())
        {
            String errorDescription = device.GetErrorDescription();
            TE_ASSERT_ERROR(false, "Can't get base texture\nError Description: " + errorDescription);
        }

        // Create texture view
        _3DTex->GetDesc(&desc);

        if (_properties.GetNumMipmaps() != (desc.MipLevels - 1))
        {
            TE_ASSERT_ERROR(false, "Driver returned different number of mip maps than requested. Requested: " + ToString(_properties.GetNumMipmaps()) + ". Got: " + ToString(desc.MipLevels - 1) + ".");
        }

        _DXGIFormat = desc.Format;

        if ((usage & TU_DEPTHSTENCIL) == 0 || readableDepth)
        {
            TEXTURE_VIEW_DESC viewDesc;
            viewDesc.MostDetailMip = 0;
            viewDesc.NumMips = desc.MipLevels;
            viewDesc.FirstArraySlice = 0;
            viewDesc.NumArraySlices = 1;
            viewDesc.Usage = GVU_DEFAULT;
            viewDesc.DebugName = debugName;

            _shaderResourceView = te_shared_ptr<D3D11TextureView>(new (te_allocate<D3D11TextureView>()) D3D11TextureView(this, viewDesc));
        }

        D3D11Utility::SetDebugName(_2DTex, debugName.c_str(), debugName.size());
    }

    void* D3D11Texture::D3D11Texture::Map(ID3D11Resource* res, D3D11_MAP flags, UINT32 mipLevel, UINT32 face, UINT32& rowPitch, UINT32& slicePitch)
    {
        D3D11_MAPPED_SUBRESOURCE pMappedResource;
        pMappedResource.pData = nullptr;

        mipLevel = Math::Clamp(mipLevel, (UINT32)mipLevel, _properties.GetNumMipmaps());
        face = Math::Clamp(face, (UINT32)0, _properties.GetNumFaces() - 1);

        if (_properties.GetTextureType() == TEX_TYPE_3D)
            face = 0;

        D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::InstancePtr());
        D3D11Device& device = rs->GetPrimaryDevice();

        {
            device.LockContext();
            _lockedSubresourceIdx = D3D11CalcSubresource(mipLevel, face, _properties.GetNumMipmaps() + 1);
            device.GetImmediateContext()->Map(res, _lockedSubresourceIdx, flags, 0, &pMappedResource);
            device.UnlockContext();
        }

        if (device.HasError())
        {
            String errorDescription = device.GetErrorDescription();
            TE_ASSERT_ERROR(false, "D3D11 device cannot map texture\nError Description: " + errorDescription);
        }

        rowPitch = pMappedResource.RowPitch;
        slicePitch = pMappedResource.DepthPitch;

        return pMappedResource.pData;
    }

    void D3D11Texture::Unmap(ID3D11Resource* res)
    {
        D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::InstancePtr());
        D3D11Device& device = rs->GetPrimaryDevice();

        device.LockContext();
        device.GetImmediateContext()->Unmap(res, _lockedSubresourceIdx);
        device.UnlockContext();

        if (device.HasError())
        {
            String errorDescription = device.GetErrorDescription();
            TE_ASSERT_ERROR(false, "D3D11 device unmap resource\nError Description: " + errorDescription);
        }
    }

    void* D3D11Texture::Mapstagingbuffer(D3D11_MAP flags, UINT32 mipLevel, UINT32 face, UINT32& rowPitch, UINT32& slicePitch)
    {
        // Note: I am creating and destroying a staging resource every time a texture is read.
        // Consider offering a flag on init that will keep this active all the time (at the cost of double memory).
        // Reading is slow operation anyway so I don't believe doing it as we are now will influence it much.

        if (!_stagingBuffer)
            CreateStagingBuffer();

        D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::InstancePtr());
        D3D11Device& device = rs->GetPrimaryDevice();
        device.LockContext();
        device.GetImmediateContext()->CopyResource(_stagingBuffer, _tex);
        device.UnlockContext();

        return Map(_stagingBuffer, flags, face, mipLevel, rowPitch, slicePitch);
    }

    void D3D11Texture::Unmapstagingbuffer()
    {
        Unmap(_stagingBuffer);
        SAFE_RELEASE(_stagingBuffer);
    }

    void* D3D11Texture::Mapstaticbuffer(PixelData lock, UINT32 mipLevel, UINT32 face)
    {
        _lockedSubresourceIdx = D3D11CalcSubresource(mipLevel, face, _properties.GetNumMipmaps() + 1);

        _staticBuffer = te_new<PixelData>(lock.GetWidth(), lock.GetHeight(), lock.GetDepth(), lock.GetFormat());
        _staticBuffer->AllocateInternalBuffer();

        return _staticBuffer->GetData();
    }

    void D3D11Texture::Unmapstaticbuffer()
    {
        UINT32 rowWidth = D3D11Mappings::GetSizeInBytes(_staticBuffer->GetFormat(), _staticBuffer->GetWidth());
        UINT32 sliceWidth = D3D11Mappings::GetSizeInBytes(_staticBuffer->GetFormat(), _staticBuffer->GetWidth(), _staticBuffer->GetHeight());

        D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::InstancePtr());
        D3D11Device& device = rs->GetPrimaryDevice();
        device.LockContext();
        device.GetImmediateContext()->UpdateSubresource(_tex, _lockedSubresourceIdx, nullptr, _staticBuffer->GetData(), rowWidth, sliceWidth);
        device.UnlockContext();

        if (device.HasError())
        {
            String errorDescription = device.GetErrorDescription();
            TE_ASSERT_ERROR(false, "D3D11 device cannot map texture\nError Description: " + errorDescription);
        }

        if (_staticBuffer != nullptr)
            te_delete(_staticBuffer);
    }

    SPtr<TextureView> D3D11Texture::CreateView(const TEXTURE_VIEW_DESC& desc)
    {
        return te_shared_ptr<D3D11TextureView>(new (te_allocate<D3D11TextureView>()) D3D11TextureView(this, desc));
    }

    /**
     * Creates a staging buffer that is used as a temporary buffer for read operations on textures that do not support
     * direct reading.
     */
    void D3D11Texture::CreateStagingBuffer()
    {
        D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::InstancePtr());
        D3D11Device& device = rs->GetPrimaryDevice();

        switch (_properties.GetTextureType())
        {
        case TEX_TYPE_1D: {
            D3D11_TEXTURE1D_DESC desc;
            _1DTex->GetDesc(&desc);

            desc.BindFlags = 0;
            desc.MiscFlags = 0;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
            desc.Usage = D3D11_USAGE_STAGING;

            device.GetD3D11Device()->CreateTexture1D(&desc, nullptr, (ID3D11Texture1D**)(&_stagingBuffer));
        } break;

        case TEX_TYPE_2D: {
            D3D11_TEXTURE2D_DESC desc;
            _2DTex->GetDesc(&desc);

            desc.BindFlags = 0;
            desc.MiscFlags = 0;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
            desc.Usage = D3D11_USAGE_STAGING;

            device.GetD3D11Device()->CreateTexture2D(&desc, nullptr, (ID3D11Texture2D**)(&_stagingBuffer));
        } break;

        case TEX_TYPE_3D: {
            D3D11_TEXTURE3D_DESC desc;
            _3DTex->GetDesc(&desc);

            desc.BindFlags = 0;
            desc.MiscFlags = 0;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
            desc.Usage = D3D11_USAGE_STAGING;

            device.GetD3D11Device()->CreateTexture3D(&desc, nullptr, (ID3D11Texture3D**)(&_stagingBuffer));
        } break;
        }
    }
}
