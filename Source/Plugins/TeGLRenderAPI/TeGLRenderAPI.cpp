#include "TeGLRenderAPI.h"
#include "TeGLRenderWindow.h"

namespace te
{
    GLRenderAPI::GLRenderAPI()
    {
    }

    GLRenderAPI::~GLRenderAPI()
    {
    }

    SPtr<RenderWindow> GLRenderAPI::CreateRenderWindow(const RENDER_WINDOW_DESC& windowDesc)
    {
        return te_shared_ptr_new<GLRenderWindow>(windowDesc);
    }

    void GLRenderAPI::Initialize()
    {
    }

    void GLRenderAPI::Update()
    {
    }
}