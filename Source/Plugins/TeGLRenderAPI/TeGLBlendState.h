#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "RenderAPI/TeBlendState.h"

namespace te
{
    /** OpenGL implementation of a blend state. */
    class GLBlendState : public BlendState
    {
    public:
        virtual ~GLBlendState();

    protected:
        friend class GLRenderStateManager;

        GLBlendState(const BLEND_STATE_DESC& desc);

        /** @copydoc BlendState::CreateInternal */
        void CreateInternal() override;
    };
}
