#include "TeGLDepthStencilState.h"
#include "TeGLRenderAPI.h"

namespace te
{
    GLDepthStencilState::GLDepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc)
        : DepthStencilState(desc)
    { }

    GLDepthStencilState::~GLDepthStencilState()
    { }

    void GLDepthStencilState::CreateInternal()
    {
        // TODO
        DepthStencilState::CreateInternal();
    }
}
