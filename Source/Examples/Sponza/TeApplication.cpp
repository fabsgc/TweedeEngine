#include "TeApplication.h"
#include "Error/TeConsole.h"
#include "Scene/TeSceneObject.h"
#include "Components/TeCCamera.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(Application)

    void Application::OnStartUp()
    {
        CoreApplication::OnStartUp();

        gSceneManager().SetMainRenderTarget(gCoreApplication().GetWindow());

        HSceneObject sceneCameraSO = SceneObject::Create("SceneCamera");
        HCamera camera = sceneCameraSO->AddComponent<CCamera>();
        camera->GetViewport()->SetClearColorValue(Color(0.17f, 0.64f, 1.0f, 1.0f));
        camera->GetViewport()->SetTarget(gCoreApplication().GetWindow());
        camera->SetMSAACount(8);
        camera->SetProjectionType(ProjectionType::PT_PERSPECTIVE);
        camera->SetMain(true);
        camera->Initialize();

        sceneCameraSO->LookAt(Vector3(0.0f, 0.0f, 0.0f));
    }

    void Application::OnShutDown()
    {
        CoreApplication::OnShutDown();
    }

    void Application::PreUpdate()
    {
        CoreApplication::PreUpdate();
    }

    void Application::PostUpdate()
    { 
        CoreApplication::PostUpdate();
    }
}
