#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "RenderAPI/TeBlendState.h"

namespace te
{
    /** DirectX 11 implementation of a blend state. Corresponds directly with a DX11 blend state object. */
    class D3D11BlendState : public BlendState
    {
    public:
        ~D3D11BlendState();

        /** Returns the internal DX11 blend state object. */
        ID3D11BlendState* GetInternal() const { return _blendState; }

    protected:
        friend class D3D11RenderStateManager;

        D3D11BlendState(const BLEND_STATE_DESC& desc);

        /** @copydoc BlendState::CreateInternal */
        void CreateInternal() override;
    
    protected:
        ID3D11BlendState* _blendState = nullptr;
    };
}
