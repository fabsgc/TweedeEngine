#include "TeApplication.h"
#include "TeEditor.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(Application)

    void Application::PostStartUp()
    {
        Editor::StartUp();
    }

    void Application::PreShutDown()
    {
        Editor::ShutDown();
    }

    void Application::PreUpdate()
    { 
        gEditor().Update();
    }

    void Application::PostUpdate()
    { }

    void Application::PostRender()
    {
        gEditor().PostRender();
    }
}
