#include "TeGLRenderAPI.h"

#if TE_PLATFORM == TE_PLATFORM_WIN32
#   include "Win32/TeWin32RenderWindow.h"
#elif TE_PLATFORM == TE_PLATFORM_LINUX
#   include "Linux/TeLinuxRenderWindow.h"
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
#if TE_PLATFORM == TE_PLATFORM_WIN32
        return te_shared_ptr_new<Win32RenderWindow>(windowDesc);
#elif TE_PLATFORM == TE_PLATFORM_LINUX
        return te_shared_ptr_new<LinuxRenderWindow>(windowDesc);
#endif
    }

    void GLRenderAPI::Initialize()
    {
    }

    void GLRenderAPI::Update()
    {
    }
}