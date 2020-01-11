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

        // TODO
        // Default depthStencilState create without using _properties (which is empty)

        depthStencilState.DepthEnable = true;
        depthStencilState.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depthStencilState.DepthFunc = D3D11_COMPARISON_LESS;
        depthStencilState.StencilEnable = true;
        depthStencilState.StencilReadMask = 0xFF;
        depthStencilState.StencilWriteMask = 0xFF;
        depthStencilState.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilState.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        depthStencilState.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilState.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        depthStencilState.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilState.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        depthStencilState.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilState.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::InstancePtr());
        D3D11Device& device = rs->GetPrimaryDevice();
        HRESULT hr = device.GetD3D11Device()->CreateDepthStencilState(&depthStencilState, &_depthStencilState);

        if (FAILED(hr) || device.HasError())
        {
            String errorDescription = device.GetErrorDescription();
            TE_ASSERT_ERROR(false, "Cannot create depth stencil state.\nError Description:" + errorDescription, __FILE__, __LINE__);
        }

        DepthStencilState::CreateInternal();
    }
}
