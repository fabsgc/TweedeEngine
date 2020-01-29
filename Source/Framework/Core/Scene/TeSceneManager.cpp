#include "TeSceneManager.h"
#include "Renderer/TeCamera.h"
#include "TeCoreApplication.h"

namespace te
{
    SceneManager::SceneManager()
    { }

    SceneManager::~SceneManager()
    {
        _cameras.clear();
    }

    void SceneManager::Initialize()
    { }

    void SceneManager::RegisterCamera(const SPtr<Camera>& camera)
    {
        _cameras[camera.get()] = camera;
    }

    void SceneManager::UnregisterCamera(const SPtr<Camera>& camera)
    {
        _cameras.erase(camera.get());
    }

    void SceneManager::SetMainRenderTarget(const SPtr<RenderTarget>& rt)
    {
        if (_mainRenderTarget == rt)
            return;

        _mainRTResizedConn.Disconnect();

        if (rt != nullptr)
            _mainRTResizedConn = rt->OnResized.Connect(std::bind(&SceneManager::OnMainRenderTargetResized, this));

        _mainRenderTarget = rt;

        float aspect = 1.0f;
        if (rt != nullptr)
        {
            auto& rtProps = rt->GetProperties();
            aspect = rtProps.Width / (float)rtProps.Height;
        }

        for (auto& entry : _cameras)
        {
            if (entry.second->IsMain())
            {
                entry.second->GetViewport()->SetTarget(rt);
                entry.second->SetAspectRatio(aspect);
            }
        }
    }

    void SceneManager::OnMainRenderTargetResized()
    {
        auto& rtProps = _mainRenderTarget->GetProperties();
        float aspect = rtProps.Width / (float)rtProps.Height;

        for (auto& entry : _cameras)
        {
            if (entry.second->IsMain())
                entry.second->SetAspectRatio(aspect);
        }
    }

    SceneManager& gSceneManager()
    {
        return SceneManager::Instance();
    }
}
