//************************************ bs::framework - Copyright 2018 Marko Pin#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "RenderAPI/TeSamplerState.h"

namespace te {
        /**	DirectX 11 implementation of a sampler state. Wraps a DX11 sampler state object. */
    class D3D11SamplerState : public SamplerState
    {
    public:
        ~D3D11SamplerState();
        ID3D11SamplerState* GetInternal() const { return _samplerState; }

    protected:
        friend class D3D11RenderStateManager;

        D3D11SamplerState(const SAMPLER_STATE_DESC& desc);

        /** @copydoc SamplerState::CreateInternal */
        void CreateInternal() override;

        ID3D11SamplerState* _samplerState = nullptr;
    };
}
