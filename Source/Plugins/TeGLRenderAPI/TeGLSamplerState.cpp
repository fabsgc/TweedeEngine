#include "TeGLSamplerState.h"
#include "TeGLRenderAPI.h"

namespace te {
    GLSamplerState::GLSamplerState(const SAMPLER_STATE_DESC& desc)
        : SamplerState(desc)
    { }

    GLSamplerState::~GLSamplerState()
    { }

    void GLSamplerState::CreateInternal()
    {
        SamplerState::CreateInternal();
    }
}
