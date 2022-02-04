#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "RenderAPI/TeRasterizerState.h"

namespace te
{
    /** OpenGL implementation of a rasterizer state. */
    class GLRasterizerState : public RasterizerState
    {
    public:
        virtual ~GLRasterizerState();

    protected:
        friend class GLRenderStateManager;

        GLRasterizerState(const RASTERIZER_STATE_DESC& desc);

        /** @copydoc RasterizerState::CreateInternal */
        void CreateInternal() override;
    };
}
