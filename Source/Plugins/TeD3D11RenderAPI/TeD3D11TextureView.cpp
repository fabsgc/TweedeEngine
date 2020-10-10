#include "TeD3D11TextureView.h"
#include "TeD3D11RenderAPI.h"
#include "TeD3D11Device.h"
#include "TeD3D11Texture.h"
#include "TeD3D11Mappings.h"

namespace te
{
    D3D11TextureView::D3D11TextureView(const D3D11Texture* texture, const TEXTURE_VIEW_DESC& desc)
        : TextureView(desc)
    {
        if ((_desc.Usage & GVU_RANDOMWRITE) != 0)
        {
            _UAV = CreateUAV(texture, _desc.MostDetailMip, _desc.FirstArraySlice, _desc.NumArraySlices);
        }
        else if ((_desc.Usage & GVU_RENDERTARGET) != 0)
        {
            _RTV = CreateRTV(texture, _desc.MostDetailMip, _desc.FirstArraySlice, _desc.NumArraySlices);
        }
        else if ((_desc.Usage & GVU_DEPTHSTENCIL) != 0)
        {
            _WDepthWStencilView = CreateDSV(texture, _desc.MostDetailMip, _desc.FirstArraySlice, _desc.NumArraySlices, false, false);
            _RODepthWStencilView = CreateDSV(texture, _desc.MostDetailMip, _desc.FirstArraySlice, _desc.NumArraySlices, true, false);
            _RODepthROStencilView = CreateDSV(texture, _desc.MostDetailMip, _desc.FirstArraySlice, _desc.NumArraySlices, true, true);
            _WDepthROStencilView = CreateDSV(texture, _desc.MostDetailMip, _desc.FirstArraySlice, _desc.NumArraySlices, false, true);
        }
        else
        {
            _SRV = CreateSRV(texture, _desc.MostDetailMip, _desc.NumMips, _desc.FirstArraySlice, _desc.NumArraySlices);
        }
    }

    D3D11TextureView::~D3D11TextureView()
    {
        SAFE_RELEASE(_SRV);
        SAFE_RELEASE(_UAV);
        SAFE_RELEASE(_RTV);
        SAFE_RELEASE(_WDepthWStencilView);
        SAFE_RELEASE(_WDepthROStencilView);
        SAFE_RELEASE(_RODepthWStencilView);
        SAFE_RELEASE(_RODepthROStencilView);
    }

    ID3D11DepthStencilView* D3D11TextureView::GetDSV(bool readOnlyDepth, bool readOnlyStencil) const
    {
        if(readOnlyDepth)
        {
            if (readOnlyStencil)
            {
                return _RODepthROStencilView;
            }
            else
            {
                return _RODepthWStencilView;
            }
        }
        else
        {
            if (readOnlyStencil)
            {
                return _WDepthROStencilView;
            }
            else
            {
                return _WDepthWStencilView;
            }
        }
    }

    ID3D11ShaderResourceView* D3D11TextureView::CreateSRV(const D3D11Texture* texture,
        UINT32 mostDetailMip, UINT32 numMips, UINT32 firstArraySlice, UINT32 numArraySlices)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC desc;
        ZeroMemory(&desc, sizeof(desc));

        const TextureProperties& texProps = texture->GetProperties();
        UINT32 numFaces = texProps.GetNumFaces();

        switch (texProps.GetTextureType())
        {
        case TEX_TYPE_1D:
            if (numFaces <= 1)
            {
                desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
                desc.Texture1D.MipLevels = numMips;
                desc.Texture1D.MostDetailedMip = mostDetailMip;
            }
            else
            {
                desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
                desc.Texture1DArray.MipLevels = numMips;
                desc.Texture1DArray.MostDetailedMip = mostDetailMip;
                desc.Texture1DArray.FirstArraySlice = firstArraySlice;
                desc.Texture1DArray.ArraySize = numArraySlices;
            }
            break;
        case TEX_TYPE_2D:
            if (texProps.GetNumSamples() > 1)
            {
                if (numFaces <= 1)
                {
                    desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
                }
                else
                {
                    desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
                    desc.Texture2DMSArray.FirstArraySlice = firstArraySlice;
                    desc.Texture2DMSArray.ArraySize = numArraySlices;
                }
            }
            else
            {
                if (numFaces <= 1)
                {
                    desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                    desc.Texture2D.MipLevels = numMips;
                    desc.Texture2D.MostDetailedMip = mostDetailMip;
                }
                else
                {
                    desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
                    desc.Texture2DArray.MipLevels = numMips;
                    desc.Texture2DArray.MostDetailedMip = mostDetailMip;
                    desc.Texture2DArray.FirstArraySlice = firstArraySlice;
                    desc.Texture2DArray.ArraySize = numArraySlices;
                }
            }
            break;
        case TEX_TYPE_3D:
            desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
            desc.Texture3D.MipLevels = numMips;
            desc.Texture3D.MostDetailedMip = mostDetailMip;
            break;
        case TEX_TYPE_CUBE_MAP:
            if(numFaces % 6 == 0)
            {
                if (numFaces == 6)
                {
                    desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
                    desc.TextureCube.MipLevels = numMips;
                    desc.TextureCube.MostDetailedMip = mostDetailMip;
                }
                else
                {
                    desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
                    desc.TextureCubeArray.MipLevels = numMips;
                    desc.TextureCubeArray.MostDetailedMip = mostDetailMip;
                    desc.TextureCubeArray.First2DArrayFace = firstArraySlice;
                    desc.TextureCubeArray.NumCubes = numArraySlices / 6;
                }
            }
            else
            {
                desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
                desc.Texture2DArray.MipLevels = numMips;
                desc.Texture2DArray.MostDetailedMip = mostDetailMip;
                desc.Texture2DArray.FirstArraySlice = firstArraySlice;
                desc.Texture2DArray.ArraySize = numArraySlices;
            }

            break;
        default:
            TE_ASSERT_ERROR(false, "Invalid texture type for this view type.");
        }

        desc.Format = texture->GetColorFormat();

        ID3D11ShaderResourceView* srv = nullptr;

        D3D11RenderAPI* d3d11rs = static_cast<D3D11RenderAPI*>(D3D11RenderAPI::InstancePtr());
        HRESULT hr = d3d11rs->GetPrimaryDevice().GetD3D11Device()->CreateShaderResourceView(texture->GetDX11Resource(), &desc, &srv);

        if (FAILED(hr) || d3d11rs->GetPrimaryDevice().HasError())
        {
            String msg = d3d11rs->GetPrimaryDevice().GetErrorDescription();
            TE_ASSERT_ERROR(false, "Cannot create ShaderResourceView: " + msg);
        }

        return srv;
    }

    ID3D11RenderTargetView* D3D11TextureView::CreateRTV(const D3D11Texture* texture,
        UINT32 mipSlice, UINT32 firstArraySlice, UINT32 numArraySlices)
    {
        D3D11_RENDER_TARGET_VIEW_DESC desc;
        ZeroMemory(&desc, sizeof(desc));

        const TextureProperties& texProps = texture->GetProperties();
        UINT32 numFaces = texProps.GetNumFaces();

        switch (texProps.GetTextureType())
        {
        case TEX_TYPE_1D:
            if (numFaces <= 1)
            {
                desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
                desc.Texture1D.MipSlice = mipSlice;
            }
            else
            {
                desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
                desc.Texture1DArray.MipSlice = mipSlice;
                desc.Texture1DArray.FirstArraySlice = firstArraySlice;
                desc.Texture1DArray.ArraySize = numArraySlices;
            }
            break;
        case TEX_TYPE_2D:
            if (texProps.GetNumSamples() > 1)
            {
                if (numFaces <= 1)
                {
                    desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
                }
                else
                {
                    desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
                    desc.Texture2DMSArray.FirstArraySlice = firstArraySlice;
                    desc.Texture2DMSArray.ArraySize = numArraySlices;
                }
            }
            else
            {
                if (numFaces <= 1)
                {
                    desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
                    desc.Texture2D.MipSlice = mipSlice;
                }
                else
                {
                    desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
                    desc.Texture2DArray.MipSlice = mipSlice;
                    desc.Texture2DArray.FirstArraySlice = firstArraySlice;
                    desc.Texture2DArray.ArraySize = numArraySlices;
                }
            }
            break;
        case TEX_TYPE_3D:
            desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
            desc.Texture3D.MipSlice = mipSlice;
            desc.Texture3D.FirstWSlice = 0;
            desc.Texture3D.WSize = texProps.GetDepth();
            break;
        case TEX_TYPE_CUBE_MAP:
            desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
            desc.Texture2DArray.FirstArraySlice = firstArraySlice;
            desc.Texture2DArray.ArraySize = numArraySlices;
            desc.Texture2DArray.MipSlice = mipSlice;
            break;
        default:
            TE_ASSERT_ERROR(false, "Invalid texture type for this view type.");
        }

        desc.Format = texture->GetColorFormat();

        ID3D11RenderTargetView* rtv = nullptr;

        D3D11RenderAPI* d3d11rs = static_cast<D3D11RenderAPI*>(D3D11RenderAPI::InstancePtr());
        HRESULT hr = d3d11rs->GetPrimaryDevice().GetD3D11Device()->CreateRenderTargetView(texture->GetDX11Resource(), &desc, &rtv);

        if (FAILED(hr) || d3d11rs->GetPrimaryDevice().HasError())
        {
            String msg = d3d11rs->GetPrimaryDevice().GetErrorDescription();
            TE_ASSERT_ERROR(false, "Cannot create RenderTargetView: " + msg);
        }

        return rtv;
    }

    ID3D11UnorderedAccessView* D3D11TextureView::CreateUAV(const D3D11Texture* texture,
        UINT32 mipSlice, UINT32 firstArraySlice, UINT32 numArraySlices)
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
        ZeroMemory(&desc, sizeof(desc));

        const TextureProperties& texProps = texture->GetProperties();
        UINT32 numFaces = texProps.GetNumFaces();

        switch (texProps.GetTextureType())
        {
        case TEX_TYPE_1D:
            if (numFaces <= 1)
            {
                desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;
                desc.Texture1D.MipSlice = mipSlice;
            }
            else
            {
                desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
                desc.Texture1DArray.MipSlice = mipSlice;
                desc.Texture1DArray.FirstArraySlice = firstArraySlice;
                desc.Texture1DArray.ArraySize = numArraySlices;
            }
            break;
        case TEX_TYPE_2D:
            if (numFaces <= 1)
            {
                desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
                desc.Texture2D.MipSlice = mipSlice;
            }
            else
            {
                desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
                desc.Texture2DArray.MipSlice = mipSlice;
                desc.Texture2DArray.FirstArraySlice = firstArraySlice;
                desc.Texture2DArray.ArraySize = numArraySlices;
            }
            break;
        case TEX_TYPE_3D:
            desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
            desc.Texture3D.MipSlice = mipSlice;
            desc.Texture3D.FirstWSlice = 0;
            desc.Texture3D.WSize = texProps.GetDepth();
            break;
        case TEX_TYPE_CUBE_MAP:
            desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
            desc.Texture2DArray.FirstArraySlice = firstArraySlice;
            desc.Texture2DArray.ArraySize = numArraySlices;
            desc.Texture2DArray.MipSlice = mipSlice;
            break;
        default:
            TE_ASSERT_ERROR(false, "Invalid texture type for this view type.");
        }

        desc.Format = texture->GetDXGIFormat();

        ID3D11UnorderedAccessView* uav = nullptr;

        D3D11RenderAPI* d3d11rs = static_cast<D3D11RenderAPI*>(D3D11RenderAPI::InstancePtr());
        HRESULT hr = d3d11rs->GetPrimaryDevice().GetD3D11Device()->CreateUnorderedAccessView(texture->GetDX11Resource(), &desc, &uav);

        if (FAILED(hr) || d3d11rs->GetPrimaryDevice().HasError())
        {
            String msg = d3d11rs->GetPrimaryDevice().GetErrorDescription();
            TE_ASSERT_ERROR(false, "Cannot create UnorderedAccessView: " + msg);
        }

        return uav;
    }

    ID3D11DepthStencilView* D3D11TextureView::CreateDSV(const D3D11Texture* texture,
        UINT32 mipSlice, UINT32 firstArraySlice, UINT32 numArraySlices, bool readOnlyDepth, bool readOnlyStencil)
    {
        D3D11_DEPTH_STENCIL_VIEW_DESC desc;
        ZeroMemory(&desc, sizeof(desc));

        const TextureProperties& texProps = texture->GetProperties();
        UINT32 numFaces = texProps.GetNumFaces();

        switch (texProps.GetTextureType())
        {
        case TEX_TYPE_1D:
            if (numFaces <= 1)
            {
                desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1D;
                desc.Texture1D.MipSlice = mipSlice;
            }
            else
            {
                desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
                desc.Texture1DArray.MipSlice = mipSlice;
                desc.Texture1DArray.FirstArraySlice = firstArraySlice;
                desc.Texture1DArray.ArraySize = numArraySlices;
            }
            break;
        case TEX_TYPE_2D:
            if (texProps.GetNumSamples() > 1)
            {
                if (numFaces <= 1)
                {
                    desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
                }
                else
                {
                    desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
                    desc.Texture2DMSArray.FirstArraySlice = firstArraySlice;
                    desc.Texture2DMSArray.ArraySize = numArraySlices;
                }
            }
            else
            {
                if (numFaces <= 1)
                {
                    desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
                    desc.Texture2D.MipSlice = mipSlice;
                }
                else
                {
                    desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
                    desc.Texture2DArray.MipSlice = mipSlice;
                    desc.Texture2DArray.FirstArraySlice = firstArraySlice;
                    desc.Texture2DArray.ArraySize = numArraySlices;
                }
            }
            break;
        case TEX_TYPE_3D:
            desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
            desc.Texture2DArray.FirstArraySlice = 0;
            desc.Texture2DArray.ArraySize = texProps.GetDepth();
            desc.Texture2DArray.MipSlice = mipSlice;
            break;
        case TEX_TYPE_CUBE_MAP:
            desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
            desc.Texture2DArray.FirstArraySlice = firstArraySlice;
            desc.Texture2DArray.ArraySize = numArraySlices;
            desc.Texture2DArray.MipSlice = mipSlice;
            break;
        default:
            TE_ASSERT_ERROR(false, "Invalid texture type for this view type.");
        }

        desc.Format = texture->GetDepthStencilFormat();

        if (readOnlyDepth)
            desc.Flags = D3D11_DSV_READ_ONLY_DEPTH;

        bool hasStencil = desc.Format == DXGI_FORMAT_D32_FLOAT_S8X24_UINT ||
            desc.Format == DXGI_FORMAT_D24_UNORM_S8_UINT;

        if (readOnlyStencil && hasStencil)
            desc.Flags |= D3D11_DSV_READ_ONLY_STENCIL;

        ID3D11DepthStencilView* dsv = nullptr;

        D3D11RenderAPI* d3d11rs = static_cast<D3D11RenderAPI*>(D3D11RenderAPI::InstancePtr());
        HRESULT hr = d3d11rs->GetPrimaryDevice().GetD3D11Device()->CreateDepthStencilView(texture->GetDX11Resource(), &desc, &dsv);

        if (FAILED(hr) || d3d11rs->GetPrimaryDevice().HasError())
        {
            String msg = d3d11rs->GetPrimaryDevice().GetErrorDescription();
            TE_ASSERT_ERROR(false, "Cannot create DepthStencilView: " + msg);
        }

        return dsv;
    }
}
