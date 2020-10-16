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
        Editor::Instance().Update();
    }

    void Application::PostUpdate()
    { }
}
