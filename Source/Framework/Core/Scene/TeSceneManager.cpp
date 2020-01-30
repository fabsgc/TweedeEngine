#include "TeSceneManager.h"
#include "Renderer/TeCamera.h"
#include "TeCoreApplication.h"

namespace te
{
    SceneInstance::SceneInstance(const String& name, const HSceneObject& root)
        : _name(name)
        , _root(root)
    { }

    SceneManager::SceneManager()
        : _mainScene(te_shared_ptr_new<SceneInstance>("Main", SceneObject::CreateInternal("SceneRoot")))
    {
        _mainScene->_root->SetScene(_mainScene);
    }

    SceneManager::~SceneManager()
    {
        if (_mainScene->_root.GetInternalPtr() != nullptr && !_mainScene->_root.IsDestroyed())
            _mainScene->_root->Destroy(true);

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

    void SceneManager::RegisterNewSO(const HSceneObject& node)
    {
        // TODO
    }

    void SceneManager::_notifyComponentCreated(const HComponent& component, bool parentActive)
    {
        // TODO
    }

    void SceneManager::_notifyComponentActivated(const HComponent& component, bool triggerEvent)
    {
        // TODO
    }

    void SceneManager::_notifyComponentDeactivated(const HComponent& component, bool triggerEvent)
    {
        // TODO
    }

    void SceneManager::_notifyComponentDestroyed(const HComponent& component, bool immediate)
    {
        // TODO
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

    void SceneManager::SetComponentState(ComponentState state)
    {
        // TODO
    }

    SceneManager& gSceneManager()
    {
        return SceneManager::Instance();
    }
}
