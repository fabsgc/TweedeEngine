#include "TeGLRenderAPI.h"

#if TE_PLATFORM == TE_PLATFORM_WIN32
#   include "Win32/TeWin32RenderWindow.h"
#   define WINDOW_TYPE Win32RenderWindow
#elif
#   include "Linux/TeLinuxRenderWindow.h"
#   define WINDOW_TYPE LinuxRenderWindow
#endif

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
        return te_shared_ptr_new<WINDOW_TYPE>(windowDesc);
    }

    void GLRenderAPI::Initialize()
    {
    }

    void GLRenderAPI::Update()
    {
    }
}