//************************************ bs::framework - Copyright 2018 Marko Pin#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "RenderAPI/TeSamplerState.h"

namespace te {
        /**	OpenGL 11 implementation of a sampler state. Wraps a DX11 sampler state object. */
    class GLSamplerState : public SamplerState
    {
    public:
        ~GLSamplerState();

    protected:
        friend class GLRenderStateManager;

        GLSamplerState(const SAMPLER_STATE_DESC& desc);

        /** @copydoc SamplerState::CreateInternal */
        void CreateInternal() override;
    };
}
