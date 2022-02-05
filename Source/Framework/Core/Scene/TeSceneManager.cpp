#include "TeSceneManager.h"
#include "Renderer/TeCamera.h"
#include "TeCoreApplication.h"
#include "Physics/TePhysics.h"
#include "Utility/TeFrameAllocator.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(SceneManager)

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

    SceneInstance::SceneInstance(const String& name, const HSceneObject& root, const SPtr<PhysicsScene>& physicsScene)
        : _name(name)
        , _root(root)
        , _physicsScene(physicsScene)
    { }

    bool SceneInstance::RayCast(const Ray& ray, PhysicsQueryHit& hit, float maxDist) const
    {
        return RayCast(ray.GetOrigin(), ray.GetDirection(), hit, maxDist);
    }

    bool SceneInstance::RayCast(const Vector3& origin, const Vector3& unitDir, PhysicsQueryHit& hit, float maxDist) const
    {
        bool isRunning = gCoreApplication().GetState().IsFlagSet(ApplicationState::Physics);
        if (gPhysics().IsPaused() || !isRunning)
            return false;

        return _physicsScene->RayCast(origin, unitDir, hit, maxDist);
    }

    bool SceneInstance::RayCast(const Ray& ray, Vector<PhysicsQueryHit>& hits, float maxDist) const
    {
        return RayCast(ray.GetOrigin(), ray.GetDirection(), hits, maxDist);
    }

    bool SceneInstance::RayCast(const Vector3& origin, const Vector3& unitDir, Vector<PhysicsQueryHit>& hits, float maxDist) const
    {
        bool isRunning = gCoreApplication().GetState().IsFlagSet(ApplicationState::Physics);
        if (gPhysics().IsPaused() || !isRunning)
            return false;

        return _physicsScene->RayCast(origin, unitDir, hits, maxDist);
    }

    SceneManager::SceneManager()
        : _mainScene(te_shared_ptr_new<SceneInstance>(
            "Main", SceneObject::CreateInternal("SceneRoot"), gPhysics().CreatePhysicsScene()))
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

    void SceneManager::ClearScene(bool forceAll)
    {
        UINT32 curIdx = 0;
        UINT32 numChildren = _mainScene->_root->GetNumChildren();

        for (UINT32 i = 0; i < numChildren; i++)
        {
            HSceneObject child = _mainScene->_root->GetChild(curIdx);
            
            if (forceAll || !child->HasFlag(SOF_Persistent))
                child->Destroy();
            else
                curIdx++;
        }

        GameObjectManager::Instance().DestroyQueuedObjects();

        HSceneObject newRoot = SceneObject::CreateInternal("SceneRoot");
        SetRootNode(newRoot);
    }

    void SceneManager::SetRootNode(const HSceneObject& root)
    {
        if (root == nullptr)
            return;

        HSceneObject oldRoot = _mainScene->_root;
        UINT32 numChildren = oldRoot->GetNumChildren();

        // Make sure to keep persistent objects
        te_frame_mark();
        {
            FrameVector<HSceneObject> toRemove;
            for (UINT32 i = 0; i < numChildren; i++)
            {
                HSceneObject child = oldRoot->GetChild(i);

                if (child->HasFlag(SOF_Persistent))
                    toRemove.push_back(child);
            }

            for (auto& entry : toRemove)
                entry->SetParent(root, false);
        }
        te_frame_clear();

        _mainScene->_root = root;
        _mainScene->_root->_setParent(HSceneObject());
        _mainScene->_root->SetScene(_mainScene);

        oldRoot->Destroy();
    }

    void SceneManager::_bindActor(const SPtr<SceneActor>& actor, const HSceneObject& so)
    {
        _boundActors[actor.get()] = BoundActorData(actor, so);
        actor->UpdateState(*so, true);
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

    void SceneManager::NotifyComponentCreated(const HComponent& component)
    {
        component->OnCreated();
        _components.push_back(component);
    }

    void SceneManager::NotifyComponentActivated(const HComponent& component, bool triggerEvent)
    {
        const bool alwaysRun = component->HasFlag(Component::AlwaysRun);
        if (alwaysRun && triggerEvent)
            component->OnEnabled();
    }

    void SceneManager::NotifyComponentDeactivated(const HComponent& component, bool triggerEvent)
    {
        const bool alwaysRun = component->HasFlag(Component::AlwaysRun);
        if (alwaysRun && triggerEvent)
            component->OnDisabled();
    }

    void SceneManager::NotifyComponentDestroyed(const HComponent& component, bool immediate)
    {
        const bool alwaysRun = component->HasFlag(Component::AlwaysRun);
        const bool isEnabled = component->SO()->GetActive() && (alwaysRun);

        if (isEnabled)
            component->OnDisabled();

        component->OnDestroyed();

        // TODO immediate not used here as every destruction is automatically immediate
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

    void SceneManager::Update()
    {
        for (auto& entry : _components)
        {
            entry->Update();
        }

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
