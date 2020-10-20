#include "TeD3D11DepthStencilState.h"
#include "TeD3D11Device.h"
#include "TeD3D11RenderAPI.h"
#include "TeD3D11Mappings.h"

namespace te
{
    D3D11DepthStencilState::D3D11DepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc)
        : DepthStencilState(desc)
    { }

    D3D11DepthStencilState::~D3D11DepthStencilState()
    {
        SAFE_RELEASE(_depthStencilState);
    }

    void D3D11DepthStencilState::CreateInternal()
    {
        D3D11_DEPTH_STENCIL_DESC depthStencilState;
        ZeroMemory(&depthStencilState, sizeof(D3D11_DEPTH_STENCIL_DESC));

        bool depthEnable = _properties.GetDepthWriteEnable() || _properties.GetDepthReadEnable();
        CompareFunction compareFunc;
        if (_properties.GetDepthReadEnable())
        {
            compareFunc = _properties.GetDepthComparisonFunc();
        }
        else
        {
            compareFunc = CMPF_ALWAYS_PASS;
        }

        depthStencilState.BackFace.StencilPassOp = D3D11Mappings::Get(_properties.GetStencilBackPassOp());
        depthStencilState.BackFace.StencilFailOp = D3D11Mappings::Get(_properties.GetStencilBackFailOp());
        depthStencilState.BackFace.StencilDepthFailOp = D3D11Mappings::Get(_properties.GetStencilBackZFailOp());
        depthStencilState.BackFace.StencilFunc = D3D11Mappings::Get(_properties.GetStencilBackCompFunc());
        depthStencilState.FrontFace.StencilPassOp = D3D11Mappings::Get(_properties.GetStencilFrontPassOp());
        depthStencilState.FrontFace.StencilFailOp = D3D11Mappings::Get(_properties.GetStencilFrontFailOp());
        depthStencilState.FrontFace.StencilDepthFailOp = D3D11Mappings::Get(_properties.GetStencilFrontZFailOp());
        depthStencilState.FrontFace.StencilFunc = D3D11Mappings::Get(_properties.GetStencilFrontCompFunc());
        depthStencilState.DepthEnable = depthEnable;
        depthStencilState.DepthWriteMask = _properties.GetDepthWriteEnable() ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
        depthStencilState.DepthFunc = D3D11Mappings::Get(compareFunc);
        depthStencilState.StencilEnable = _properties.GetStencilEnable();
        depthStencilState.StencilReadMask = _properties.GetStencilReadMask();
        depthStencilState.StencilWriteMask = _properties.GetStencilWriteMask();

        D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::InstancePtr());
        D3D11Device& device = rs->GetPrimaryDevice();
        HRESULT hr = device.GetD3D11Device()->CreateDepthStencilState(&depthStencilState, &_depthStencilState);

        if (FAILED(hr) || device.HasError())
        {
            String errorDescription = device.GetErrorDescription();
            TE_ASSERT_ERROR(false, "Cannot create depth stencil state.\nError Description: " + errorDescription);
        }

        DepthStencilState::CreateInternal();
    }
}
