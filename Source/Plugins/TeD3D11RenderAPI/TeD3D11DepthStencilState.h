#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "RenderAPI/TeDepthStencilState.h"

namespace te
{
    /** DirectX 11 implementation of a depth stencil state. Corresponds directly with a DX11 depth stencil state object. */
    class D3D11DepthStencilState : public DepthStencilState
    {
    public:
        ~D3D11DepthStencilState();

        /** Returns the internal DX11 depth stencil state object. */
        ID3D11DepthStencilState* GetInternal() const { return _depthStencilState; }

    protected:
        friend class D3D11RenderStateManager;

        D3D11DepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc);

        /** @copydoc DepthStencilState::CreateInternal */
        void CreateInternal() override;

        ID3D11DepthStencilState* _depthStencilState = nullptr;
    };
}
