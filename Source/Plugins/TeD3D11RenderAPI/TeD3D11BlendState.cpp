#include "TeD3D11BlendState.h"
#include "TeD3D11Mappings.h"
#include "TeD3D11RenderAPI.h"
#include "TeD3D11Device.h"

namespace te
{
    D3D11BlendState::D3D11BlendState(const BLEND_STATE_DESC& desc)
        : BlendState(desc)
    { }

    D3D11BlendState::~D3D11BlendState()
    {
        SAFE_RELEASE(_blendState);
    }

    void D3D11BlendState::CreateInternal()
    {
        D3D11_BLEND_DESC blendStateDesc;
        ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));

        blendStateDesc.AlphaToCoverageEnable = _properties.GetAlphaToCoverageEnabled();
        blendStateDesc.IndependentBlendEnable = _properties.GetIndependantBlendEnable();

        for(UINT32 i = 0; i < TE_MAX_MULTIPLE_RENDER_TARGETS; i++)
        {
            blendStateDesc.RenderTarget[i].BlendEnable = _properties.GetBlendEnabled(i);
            blendStateDesc.RenderTarget[i].BlendOp = D3D11Mappings::Get(_properties.GetBlendOperation(i));
            blendStateDesc.RenderTarget[i].BlendOpAlpha = D3D11Mappings::Get(_properties.GetAlphaBlendOperation(i));
            blendStateDesc.RenderTarget[i].DestBlend = D3D11Mappings::Get(_properties.GetDstBlend(i));
            blendStateDesc.RenderTarget[i].DestBlendAlpha = D3D11Mappings::Get(_properties.GetAlphaDstBlend(i));
            blendStateDesc.RenderTarget[i].RenderTargetWriteMask = 0xf & (_properties.GetRenderTargetWriteMask(i)); // Mask out all but last 4 bits
            blendStateDesc.RenderTarget[i].SrcBlend = D3D11Mappings::Get(_properties.GetSrcBlend(i));
            blendStateDesc.RenderTarget[i].SrcBlendAlpha = D3D11Mappings::Get(_properties.GetAlphaSrcBlend(i));
        }

        D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::InstancePtr());
        D3D11Device& device = rs->GetPrimaryDevice();
        HRESULT hr = device.GetD3D11Device()->CreateBlendState(&blendStateDesc, &_blendState);

        if(FAILED(hr) || device.HasError())
        {
            String errorDescription = device.GetErrorDescription();
            TE_ASSERT_ERROR(false, "Cannot create blend state. Error Description: " + errorDescription);
        }

        BlendState::CreateInternal();
    }
}
