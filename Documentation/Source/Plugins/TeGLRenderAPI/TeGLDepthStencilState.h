#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "RenderAPI/TeDepthStencilState.h"

namespace te
{
    /** OpenGL implementation of a depth stencil state. */
    class GLDepthStencilState : public DepthStencilState
    {
    public:
        ~GLDepthStencilState();

    protected:
        friend class GLRenderStateManager;

        GLDepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc);

        /** @copydoc DepthStencilState::CreateInternal() */
        void CreateInternal() override;
    };
}
