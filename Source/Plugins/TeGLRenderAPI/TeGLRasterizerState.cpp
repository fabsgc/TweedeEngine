#include "TeGLRasterizerState.h"
#include "TeGLRenderAPI.h"
#include "Math/TeMath.h"

namespace te
{
    GLRasterizerState::GLRasterizerState(const RASTERIZER_STATE_DESC& desc)
        : RasterizerState(desc)
    { }

    GLRasterizerState::~GLRasterizerState()
    { }

    void GLRasterizerState::CreateInternal()
    {
        // TODO
        RasterizerState::CreateInternal();
    }
}
