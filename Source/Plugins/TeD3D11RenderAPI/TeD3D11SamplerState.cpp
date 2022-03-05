#include "TeD3D11SamplerState.h"
#include "TeD3D11RenderAPI.h"
#include "TeD3D11Device.h"
#include "TeD3D11Mappings.h"
#include "TeD3D11Utility.h"

namespace te {
    D3D11SamplerState::D3D11SamplerState(const SAMPLER_STATE_DESC& desc)
        : SamplerState(desc)
    { }

    D3D11SamplerState::~D3D11SamplerState()
    {
        SAFE_RELEASE(_samplerState);
    }

    void D3D11SamplerState::CreateInternal()
    {
        D3D11_SAMPLER_DESC samplerState;
        ZeroMemory(&samplerState, sizeof(D3D11_SAMPLER_DESC));

        samplerState.AddressU = D3D11Mappings::Get(_properties.GetTextureAddressingMode().u);
        samplerState.AddressV = D3D11Mappings::Get(_properties.GetTextureAddressingMode().v);
        samplerState.AddressW = D3D11Mappings::Get(_properties.GetTextureAddressingMode().w);
        samplerState.BorderColor[0] = _properties.GetBorderColor()[0];
        samplerState.BorderColor[1] = _properties.GetBorderColor()[1];
        samplerState.BorderColor[2] = _properties.GetBorderColor()[2];
        samplerState.BorderColor[3] = _properties.GetBorderColor()[3];
        samplerState.ComparisonFunc = D3D11Mappings::Get(_properties.GetComparisonFunction());
        samplerState.MaxAnisotropy = _properties.GetTextureAnisotropy();
        samplerState.MaxLOD = _properties.GetMaximumMip();
        samplerState.MinLOD = _properties.GetMinimumMip();
        samplerState.MipLODBias = _properties.GetTextureMipmapBias();

        FilterOptions minFilter = _properties.GetTextureFiltering(FT_MIN);
        FilterOptions magFilter = _properties.GetTextureFiltering(FT_MAG);
        FilterOptions mipFilter = _properties.GetTextureFiltering(FT_MIP);

        if (minFilter == FO_ANISOTROPIC && magFilter == FO_ANISOTROPIC && mipFilter == FO_ANISOTROPIC)
        {
            samplerState.Filter = D3D11_FILTER_ANISOTROPIC;
        }
        else
        {
            if (minFilter == FO_POINT || minFilter == FO_NONE)
            {
                if (magFilter == FO_POINT || magFilter == FO_NONE)
                {
                    if (mipFilter == FO_POINT || mipFilter == FO_NONE)
                        samplerState.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
                    else if (mipFilter == FO_LINEAR)
                        samplerState.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
                }
                else if (magFilter == FO_LINEAR)
                {
                    if (mipFilter == FO_POINT || mipFilter == FO_NONE)
                        samplerState.Filter = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
                    else if (mipFilter == FO_LINEAR)
                        samplerState.Filter = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
                }
            }
            else if (minFilter == FO_LINEAR)
            {
                if (magFilter == FO_POINT || magFilter == FO_NONE)
                {
                    if (mipFilter == FO_POINT || mipFilter == FO_NONE)
                        samplerState.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
                    else if (mipFilter == FO_LINEAR)
                        samplerState.Filter = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
                }
                else if (magFilter == FO_LINEAR)
                {
                    if (mipFilter == FO_POINT || mipFilter == FO_NONE)
                        samplerState.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
                    else if (mipFilter == FO_LINEAR)
                        samplerState.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
                }
            }
        }

        bool isComparison = _properties.GetComparisonFunction() != CMPF_ALWAYS_PASS;
        if (isComparison)
        {
            // Adds COMPARISON flag to the filter
            // See: http://msdn.microsoft.com/en-us/library/windows/desktop/ff476132(v=vs.85).aspx
            samplerState.Filter = (D3D11_FILTER)(0x80 | samplerState.Filter);
        }

        D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::InstancePtr());
        D3D11Device& device = rs->GetPrimaryDevice();
        HRESULT hr = device.GetD3D11Device()->CreateSamplerState(&samplerState, &_samplerState);

        if (FAILED(hr) || device.HasError())
        {
            String errorDescription = device.GetErrorDescription();
            TE_ASSERT_ERROR(false, "Cannot create sampler state.\nError Description: " + errorDescription);
        }

#if  TE_DEBUG_MODE == TE_DEBUG_ENABLED
        static String debugName = "[SAMPLER]";
        D3D11Utility::SetDebugName(_samplerState, debugName.c_str(), debugName.size());
#endif

        SamplerState::CreateInternal();
    }
}
