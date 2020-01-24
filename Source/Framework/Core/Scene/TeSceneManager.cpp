#include "TeSceneManager.h"
#include "Renderer/TeCamera.h"
#include "TeCoreApplication.h"

namespace te
{
    SceneManager::SceneManager()
    {
    }

    SceneManager::~SceneManager()
    {
        _cameras.clear();
    }

    void SceneManager::Initialize()
    {
    }

    void SceneManager::RegisterCamera(const SPtr<Camera>& camera)
    {
        _cameras[camera.get()] = camera;
    }

    void SceneManager::UnregisterCamera(const SPtr<Camera>& camera)
    {
        _cameras.erase(camera.get());
    }

    SceneManager& gSceneManager()
    {
        return SceneManager::Instance();
    }
}
