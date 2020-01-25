#include "TeD3D11GpuBufferView.h"
#include "TeD3D11GpuBuffer.h"
#include "TeD3D11RenderAPI.h"
#include "TeD3D11Device.h"
#include "TeD3D11Mappings.h"

namespace te
{
    size_t GpuBufferView::HashFunction::operator()(const GPU_BUFFER_VIEW_DESC& key) const
    {
        size_t seed = 0;
        te_hash_combine(seed, key.ElementWidth);
        te_hash_combine(seed, key.FirstElement);
        te_hash_combine(seed, key.NumElements);
        te_hash_combine(seed, key.UseCounter);
        te_hash_combine(seed, key.Usage);
        te_hash_combine(seed, key.Format);

        return seed;
    }

    bool GpuBufferView::EqualFunction::operator()
        (const GPU_BUFFER_VIEW_DESC& a, const GPU_BUFFER_VIEW_DESC& b) const
    {
        return a.ElementWidth == b.ElementWidth && a.FirstElement == b.FirstElement && a.NumElements == b.NumElements
            && a.UseCounter == b.UseCounter && a.Usage == b.Usage && a.Format == b.Format;
    }

    GpuBufferView::~GpuBufferView()
    {
        SAFE_RELEASE(_SRV);
        SAFE_RELEASE(_UAV);
    }

    void GpuBufferView::Initialize(D3D11GpuBuffer* buffer, GPU_BUFFER_VIEW_DESC& desc)
    {
        _buffer = buffer;
        _desc = desc;

        if ((desc.Usage & GVU_DEFAULT) != 0)
            _SRV = CreateSRV(buffer, desc.FirstElement, desc.ElementWidth, desc.NumElements);

        if ((desc.Usage & GVU_RANDOMWRITE) != 0)
            _UAV = CreateUAV(buffer, desc.FirstElement, desc.NumElements, desc.UseCounter);

        if ((desc.Usage & GVU_RENDERTARGET) != 0 || (desc.Usage & GVU_DEPTHSTENCIL) != 0)
        {
            TE_ASSERT_ERROR(false, "Invalid usage flags for a GPU buffer view.", __FILE__, __LINE__);
        }
    }

    ID3D11ShaderResourceView* GpuBufferView::CreateSRV(D3D11GpuBuffer* buffer, UINT32 firstElement, UINT32 elementWidth, UINT32 numElements)
    {
        const GpuBufferProperties& props = buffer->GetProperties();

        D3D11_SHADER_RESOURCE_VIEW_DESC desc;
        ZeroMemory(&desc, sizeof(desc));

        if (props.GetType() == GBT_STANDARD)
        {
            desc.Format = D3D11Mappings::GetBF(props.GetFormat());
            desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
            desc.Buffer.FirstElement = firstElement;
            desc.Buffer.NumElements = numElements;
        }
        else if (props.GetType() == GBT_STRUCTURED)
        {
            desc.Format = DXGI_FORMAT_UNKNOWN;
            desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
            desc.Buffer.FirstElement = firstElement;
            desc.Buffer.NumElements = numElements;
        }
        else if (props.GetType() == GBT_INDIRECTARGUMENT)
        {
            desc.Format = DXGI_FORMAT_R32_UINT;
            desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
            desc.Buffer.ElementOffset = firstElement * elementWidth;
            desc.Buffer.ElementWidth = elementWidth;
        }

        ID3D11ShaderResourceView* srv = nullptr;

        D3D11RenderAPI* d3d11rs = static_cast<D3D11RenderAPI*>(D3D11RenderAPI::InstancePtr());
        HRESULT hr = d3d11rs->GetPrimaryDevice().GetD3D11Device()->CreateShaderResourceView(buffer->GetDX11Buffer(), &desc, &srv);

        if (FAILED(hr) || d3d11rs->GetPrimaryDevice().HasError())
        {
            String msg = d3d11rs->GetPrimaryDevice().GetErrorDescription();
            TE_ASSERT_ERROR(false, "Cannot create ShaderResourceView: " + msg, __FILE__, __LINE__);
        }

        return srv;
    }

    ID3D11UnorderedAccessView* GpuBufferView::CreateUAV(D3D11GpuBuffer* buffer, UINT32 firstElement, UINT32 numElements,
        bool useCounter)
    {
        const GpuBufferProperties& props = buffer->GetProperties();

        D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
        ZeroMemory(&desc, sizeof(desc));

        desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

        if (props.GetType() == GBT_STANDARD)
        {
            desc.Format = D3D11Mappings::GetBF(props.GetFormat());
            desc.Buffer.FirstElement = firstElement;
            desc.Buffer.NumElements = numElements;

            if (useCounter)
                desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
            else
                desc.Buffer.Flags = 0;
        }
        else if (props.GetType() == GBT_STRUCTURED)
        {
            desc.Format = DXGI_FORMAT_UNKNOWN;
            desc.Buffer.FirstElement = firstElement;
            desc.Buffer.NumElements = numElements;

            if (useCounter)
                desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
            else
                desc.Buffer.Flags = 0;
        }
        else if (props.GetType() == GBT_INDIRECTARGUMENT)
        {
            desc.Format = DXGI_FORMAT_R32_UINT;
            desc.Buffer.Flags = 0;
            desc.Buffer.FirstElement = firstElement;
            desc.Buffer.NumElements = numElements;
        }

        ID3D11UnorderedAccessView* uav = nullptr;

        D3D11RenderAPI* d3d11rs = static_cast<D3D11RenderAPI*>(D3D11RenderAPI::InstancePtr());
        HRESULT hr = d3d11rs->GetPrimaryDevice().GetD3D11Device()->CreateUnorderedAccessView(buffer->GetDX11Buffer(), &desc, &uav);

        if (FAILED(hr) || d3d11rs->GetPrimaryDevice().HasError())
        {
            String msg = d3d11rs->GetPrimaryDevice().GetErrorDescription();
            TE_ASSERT_ERROR(false, "Cannot create UnorderedAccessView: " + msg, __FILE__, __LINE__);
        }

        return uav;
    }
}
