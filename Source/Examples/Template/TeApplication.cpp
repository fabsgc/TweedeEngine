#include "TeApplication.h"

#include "Renderer/TeCamera.h"

#include "Scene/TeSceneManager.h"

#include "Scene/TeSceneObject.h"
#include "Components/TeCCamera.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(Application)

        void Application::PostStartUp()
    {
#if TE_PLATFORM == TE_PLATFORM_WIN32
        // ######################################################
        _sceneCameraSO = SceneObject::Create("SceneCamera");
        _sceneCamera = _sceneCameraSO->AddComponent<CCamera>();
        _sceneCamera->GetViewport()->SetClearColorValue(Color(0.17f, 0.64f, 1.0f, 1.0f));
        _sceneCamera->GetViewport()->SetTarget(gCoreApplication().GetWindow());
        _sceneCamera->SetMain(true);
        _sceneCamera->Initialize();
        // ######################################################

        // ######################################################
        gSceneManager().SetMainRenderTarget(gCoreApplication().GetWindow());
        // ######################################################
#endif
    }

    void Application::PreShutDown()
    { }

    void Application::PreUpdate()
    { }

    void Application::PostUpdate()
    { }
}
