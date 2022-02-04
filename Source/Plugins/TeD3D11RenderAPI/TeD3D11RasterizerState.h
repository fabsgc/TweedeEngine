#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "RenderAPI/TeRasterizerState.h"

namespace te
{
    /** DirectX 11 implementation of a rasterizer state. Wraps a DX11 rasterizer state object. */
    class D3D11RasterizerState : public RasterizerState
    {
    public:
        virtual ~D3D11RasterizerState();
        ID3D11RasterizerState* GetInternal() const { return _rasterizerState; }

    protected:
        friend class D3D11RenderStateManager;

        D3D11RasterizerState(const RASTERIZER_STATE_DESC& desc);

        /** @copydoc RasterizerState::CreateInternal */
        void CreateInternal() override;

        ID3D11RasterizerState* _rasterizerState = nullptr;
    };
}
