#include "TeApplication.h"
#include "TeEditor.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(Application)

    void Application::PostStartUp()
    {
#if TE_PLATFORM == TE_PLATFORM_WIN32
        Editor::StartUp();
#endif
    }

    void Application::PreShutDown()
    {
#if TE_PLATFORM == TE_PLATFORM_WIN32
        Editor::ShutDown();
#endif
    }

    void Application::PreUpdate()
    {
#if TE_PLATFORM == TE_PLATFORM_WIN32
        gEditor().Update();
#endif
    }

    void Application::PostUpdate()
    { }

    void Application::PostRender()
    {
#if TE_PLATFORM == TE_PLATFORM_WIN32
        gEditor().PostRender();
#endif
    }
}
