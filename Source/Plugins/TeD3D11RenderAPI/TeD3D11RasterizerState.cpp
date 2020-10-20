#include "TeD3D11RasterizerState.h"
#include "TeD3D11RenderAPI.h"
#include "TeD3D11Device.h"
#include "TeD3D11Mappings.h"
#include "Math/TeMath.h"

namespace te
{
    D3D11RasterizerState::D3D11RasterizerState(const RASTERIZER_STATE_DESC& desc)
        : RasterizerState(desc)
    { }

    D3D11RasterizerState::~D3D11RasterizerState()
    {
        SAFE_RELEASE(_rasterizerState);
    }

    void D3D11RasterizerState::CreateInternal()
    {
        INT32 scaledDepthBias = Math::FloorToInt(-_properties.GetDepthBias() * float((1 << 24))); // Note: Assumes 24-bit depth buffer

        D3D11_RASTERIZER_DESC rasterizerStateDesc;
        ZeroMemory(&rasterizerStateDesc, sizeof(D3D11_RASTERIZER_DESC));

        rasterizerStateDesc.CullMode = D3D11Mappings::Get(_properties.GetCullMode());
        rasterizerStateDesc.FillMode = D3D11Mappings::Get(_properties.GetPolygonMode());
        rasterizerStateDesc.DepthBias = scaledDepthBias;
        rasterizerStateDesc.DepthBiasClamp = _properties.GetDepthBiasClamp();
        rasterizerStateDesc.DepthClipEnable = _properties.GetDepthClipEnable();
        rasterizerStateDesc.MultisampleEnable = _properties.GetMultisampleEnable();
        rasterizerStateDesc.ScissorEnable = _properties.GetScissorEnable();
        rasterizerStateDesc.SlopeScaledDepthBias = _properties.GetSlopeScaledDepthBias();
        rasterizerStateDesc.FrontCounterClockwise = false;

        D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::InstancePtr());
        D3D11Device& device = rs->GetPrimaryDevice();
        HRESULT hr = device.GetD3D11Device()->CreateRasterizerState(&rasterizerStateDesc, &_rasterizerState);

        if(FAILED(hr) || device.HasError())
        {
            String errorDescription = device.GetErrorDescription();
            TE_ASSERT_ERROR(false, "Cannot create rasterizer state.\nError Description: " + errorDescription);
        }

        RasterizerState::CreateInternal();
    }
}
