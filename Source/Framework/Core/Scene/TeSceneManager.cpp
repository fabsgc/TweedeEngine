#include "TeSceneManager.h"
#include "Renderer/TeCamera.h"
#include "TeCoreApplication.h"

namespace te
{
    enum ListType
    {
        NoList = 0,
        ActiveList = 1,
        InactiveList = 2,
        UninitializedList = 3
    };

    struct ScopeToggle
    {
        explicit ScopeToggle(bool& val) : val(val) { val = true; }
        ~ScopeToggle() { val = false; }

    private:
        bool& val;
    };

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

        _mainCameras.clear();
        _cameras.clear();
        _components.clear();
        _mainRTResizedConn.Disconnect();
    }

    void SceneManager::ClearScene()
    {
        UINT32 numChildren = _mainScene->_root->GetNumChildren();
        for (UINT32 i = 0; i < numChildren; i++)
        {
            HSceneObject child = _mainScene->_root->GetChild(i);
            child->Destroy();
        }

        GameObjectManager::Instance().DestroyQueuedObjects();

        HSceneObject newRoot = SceneObject::CreateInternal("SceneRoot");
        _setRootNode(newRoot);
    }

    void SceneManager::_setRootNode(const HSceneObject& root)
    {
        if (root == nullptr)
            return;

        HSceneObject oldRoot = _mainScene->_root;

        _mainScene->_root = root;
        _mainScene->_root->_setParent(HSceneObject());
        _mainScene->_root->SetScene(_mainScene);

        oldRoot->Destroy();
    }

    void SceneManager::_bindActor(const SPtr<SceneActor>& actor, const HSceneObject& so)
    {
        _boundActors[actor.get()] = BoundActorData(actor, so);
        actor->_updateState(*so, true);
    }

    void SceneManager::_unbindActor(const SPtr<SceneActor>& actor)
    {
        _boundActors.erase(actor.get());
    }

    HSceneObject SceneManager::_getActorSO(const SPtr<SceneActor>& actor) const
    {
        auto iterFind = _boundActors.find(actor.get());
        if (iterFind != _boundActors.end())
            return iterFind->second.So;

        return HSceneObject();
    }

    void SceneManager::_registerCamera(const SPtr<Camera>& camera)
    {
        _cameras[camera.get()] = camera;
    }

    void SceneManager::_unregisterCamera(const SPtr<Camera>& camera)
    {
        _cameras.erase(camera.get());

        auto iterFind = std::find_if(_mainCameras.begin(), _mainCameras.end(),
        [&](const SPtr<Camera>& x)
        {
            return x == camera;
        });

        if (iterFind != _mainCameras.end())
        {
            _mainCameras.erase(iterFind);
        }
    }

    void SceneManager::_notifyMainCameraStateChanged(const SPtr<Camera>& camera)
    {
        auto iterFind = std::find_if(_mainCameras.begin(), _mainCameras.end(),
        [&](const SPtr<Camera>& entry)
        {
            return entry == camera;
        });

        SPtr<Viewport> viewport = camera->GetViewport();
        if (camera->IsMain())
        {
            if (iterFind == _mainCameras.end())
                _mainCameras.push_back(_cameras[camera.get()]);

            viewport->SetTarget(_mainRenderTarget);
        }
        else
        {
            if (iterFind != _mainCameras.end())
                _mainCameras.erase(iterFind);

            if (viewport->GetTarget() == _mainRenderTarget)
                viewport->SetTarget(nullptr);
        }
    }

    void SceneManager::_updateCoreObjectTransforms()
    {
        for (auto& entry : _boundActors)
            entry.second.Actor->_updateState(*entry.second.So);
    }

    SPtr<Camera> SceneManager::GetMainCamera() const
    {
        if (_mainCameras.size() > 0)
            return _mainCameras[0];

        return nullptr;
    }

    HSceneObject SceneManager::GetRootNode() const
    {
        return _mainScene->GetRoot();
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

    void SceneManager::_notifyComponentCreated(const HComponent& component)
    {
        component->OnCreated();
        _components.push_back(component);
    }

    void SceneManager::_notifyComponentDestroyed(const HComponent& component)
    {
        component->OnDestroyed();

        auto co = std::find(_components.begin(), _components.end(), component);
        if (co != _components.end())
        {
            _components.erase(co);
        }
    }

    bool SceneManager::IsComponentOfType(const HComponent& component, UINT32 id)
    {
        return component->GetCoreType() == id;
    }

    void SceneManager::_update()
    {
        for (auto& entry : _components)
            entry->Update();

        GameObjectManager::Instance().DestroyQueuedObjects();
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
    void SceneManager::RegisterNewSO(const HSceneObject& node)
    {
        if (_mainScene->GetRoot())
            node->SetParent(_mainScene->GetRoot());
    }

    SceneManager& gSceneManager()
    {
        return SceneManager::Instance();
    }
}
