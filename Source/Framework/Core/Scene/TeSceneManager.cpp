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
        ScopeToggle(bool& val) :val(val) { val = true; }
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

    void SceneManager::SetComponentState(ComponentState state)
    {
        if (_disableStateChange)
        {
            TE_DEBUG("Component state cannot be changed from the calling locating. Are you calling it from Component callbacks?", __FILE__, __LINE__);
            return;
        }

        if (_componentState == state)
            return;

        ComponentState oldState = _componentState;

        // Make sure to change the state before calling any callbacks, so callbacks can query the state
        _componentState = state;

        // Make sure the per-state lists are up-to-date
        ProcessStateChanges();

        ScopeToggle toggle(_disableStateChange);

        // Wake up all components with onInitialize/onEnable events if moving to running or paused state
        if (state == ComponentState::Running || state == ComponentState::Paused)
        {
            if (oldState == ComponentState::Stopped)
            {
                // Disable, and then re-enable components that have an AlwaysRun flag
                for (auto& entry : _activeComponents)
                {
                    if (entry->GetSceneObject()->GetActive())
                    {
                        entry->OnDisabled();
                        entry->OnEnabled();
                    }
                }

                // Process any state changes queued by the component callbacks
                ProcessStateChanges();

                // Trigger enable on all components that don't have AlwaysRun flag (at this point those will be all
                // inactive components that have active scene object parents)
                for (auto& entry : _inactiveComponents)
                {
                    if (entry->GetSceneObject()->GetActive())
                    {
                        entry->OnEnabled();

                        if (state == ComponentState::Running)
                            _stateChanges.emplace_back(entry, ComponentStateEventType::Activated);
                    }
                }

                // Process any state changes queued by the component callbacks
                ProcessStateChanges();

                // Initialize and enable uninitialized components
                for (auto& entry : _uninitializedComponents)
                {
                    entry->OnInitialized();

                    if (entry->GetSceneObject()->GetActive())
                    {
                        entry->OnEnabled();
                        _stateChanges.emplace_back(entry, ComponentStateEventType::Activated);
                    }
                    else
                        _stateChanges.emplace_back(entry, ComponentStateEventType::Deactivated);
                }

                // Process any state changes queued by the component callbacks
                ProcessStateChanges();
            }
        }

        // Stop updates on all active components
        if (state == ComponentState::Paused || state == ComponentState::Stopped)
        {
            // Trigger onDisable events if stopping
            if (state == ComponentState::Stopped)
            {
                for (const auto& component : _activeComponents)
                {
                    const bool alwaysRun = component->HasFlag((UINT32)ComponentFlag::AlwaysRun);

                    component->OnDisabled();

                    if (alwaysRun)
                        component->OnEnabled();
                }
            }

            // Move from active to inactive list
            for (INT32 i = 0; i < (INT32)_activeComponents.size(); i++)
            {
                // Note: Purposely not a reference since the list changes in the add/remove methods below
                const HComponent component = _activeComponents[i];

                const bool alwaysRun = component->HasFlag((UINT32)ComponentFlag::AlwaysRun);
                if (alwaysRun)
                    continue;

                RemoveFromStateList(component);
                AddToStateList(component, InactiveList);

                i--; // Keep the same index next iteration to process the component we just swapped
            }
        }
    }

    void SceneManager::_notifyComponentCreated(const HComponent& component, bool parentActive)
    {
        // Note: This method must remain reentrant (in case the callbacks below trigger component state changes)

        // Queue the change before any callbacks trigger, as the callbacks could trigger their own changes and they should
        // be in order
        _stateChanges.emplace_back(component, ComponentStateEventType::Created);
        ScopeToggle toggle(_disableStateChange);

        component->OnCreated();

        const bool alwaysRun = component->HasFlag((UINT32)ComponentFlag::AlwaysRun);
        if (alwaysRun || _componentState != ComponentState::Stopped)
        {
            component->OnInitialized();

            if (parentActive)
                component->OnEnabled();
        }
    }

    void SceneManager::_notifyComponentActivated(const HComponent& component, bool triggerEvent)
    {
        // Note: This method must remain reentrant (in case the callbacks below trigger component state changes)

        // Queue the change before any callbacks trigger, as the callbacks could trigger their own changes and they should
        // be in order
        _stateChanges.emplace_back(component, ComponentStateEventType::Activated);
        ScopeToggle toggle(_disableStateChange);

        const bool alwaysRun = component->HasFlag((UINT32)ComponentFlag::AlwaysRun);
        if (alwaysRun || _componentState != ComponentState::Stopped)
        {
            if (triggerEvent)
                component->OnEnabled();
        }
    }

    void SceneManager::_notifyComponentDeactivated(const HComponent& component, bool triggerEvent)
    {
        // Note: This method must remain reentrant (in case the callbacks below trigger component state changes)

        // Queue the change before any callbacks trigger, as the callbacks could trigger their own changes and they should
        // be in order
        _stateChanges.emplace_back(component, ComponentStateEventType::Deactivated);
        ScopeToggle toggle(_disableStateChange);

        const bool alwaysRun = component->HasFlag((UINT32)ComponentFlag::AlwaysRun);
        if (alwaysRun || _componentState != ComponentState::Stopped)
        {
            if (triggerEvent)
                component->OnDisabled();
        }
    }

    void SceneManager::_notifyComponentDestroyed(const HComponent& component, bool immediate)
    {
        // Note: This method must remain reentrant (in case the callbacks below trigger component state changes)

        // Queue the change before any callbacks trigger, as the callbacks could trigger their own changes and they should
        // be in order
        if (!immediate)
        {
            // If destruction is immediate no point in queuing state change since it will be ignored anyway
            _stateChanges.emplace_back(component, ComponentStateEventType::Destroyed);
        }

        ScopeToggle toggle(_disableStateChange);

        const bool alwaysRun = component->HasFlag((UINT32)ComponentFlag::AlwaysRun);
        const bool isEnabled = component->GetSceneObject()->GetActive() && (alwaysRun ||
            _componentState != ComponentState::Stopped);

        if (isEnabled)
            component->OnDisabled();

        component->OnDestroyed();

        if (immediate)
        {
            // Since the state change wasn't queued, remove the component from the list right away. Its expected the caller
            // knows what is he doing.

            UINT32 existingListType;
            UINT32 existingIdx;
            DecodeComponentId(component->GetSceneManagerId(), existingIdx, existingListType);

            if (existingListType != 0)
                RemoveFromStateList(component);
        }
    }

    void SceneManager::AddToStateList(const HComponent& component, UINT32 listType)
    {
        if (listType == 0)
            return;

        Vector<HComponent>& list = *_componentsPerState[listType - 1];

        const auto idx = (UINT32)list.size();
        list.push_back(component);

        component->SetSceneManagerId(EncodeComponentId(idx, listType));
    }

    void SceneManager::RemoveFromStateList(const HComponent& component)
    {
        UINT32 listType;
        UINT32 idx;
        DecodeComponentId(component->GetSceneManagerId(), idx, listType);

        if (listType == 0)
            return;

        Vector<HComponent>& list = *_componentsPerState[listType - 1];

        UINT32 lastIdx;
        DecodeComponentId(list.back()->GetSceneManagerId(), lastIdx, listType);

        assert(list[idx] == component);

        if (idx != lastIdx)
        {
            std::swap(list[idx], list[lastIdx]);
            list[idx]->SetSceneManagerId(EncodeComponentId(idx, listType));
        }

        list.erase(list.end() - 1);
    }

    void SceneManager::ProcessStateChanges()
    {
        const bool isStopped = _componentState == ComponentState::Stopped;

        for (auto& entry : _stateChanges)
        {
            const HComponent& component = entry.Obj;
            if (component.IsDestroyed(false))
                continue;

            UINT32 existingListType;
            UINT32 existingIdx;
            DecodeComponentId(component->GetSceneManagerId(), existingIdx, existingListType);

            const bool alwaysRun = component->HasFlag((UINT32)ComponentFlag::AlwaysRun);
            const bool isActive = component->SO()->GetActive();

            UINT32 listType = 0;
            switch (entry.Type)
            {
            case ComponentStateEventType::Created:
                if (alwaysRun || !isStopped)
                    listType = isActive ? ActiveList : InactiveList;
                else
                    listType = UninitializedList;
                break;
            case ComponentStateEventType::Activated:
            case ComponentStateEventType::Deactivated:
                if (alwaysRun || !isStopped)
                    listType = isActive ? ActiveList : InactiveList;
                else
                    listType = (existingListType == UninitializedList) ? UninitializedList : InactiveList;
                break;
            case ComponentStateEventType::Destroyed:
                listType = 0;
                break;
            default: break;
            }

            if (existingListType == listType)
                continue;

            if (existingListType != 0)
                RemoveFromStateList(component);

            AddToStateList(component, listType);
        }

        _stateChanges.clear();
    }


    UINT32 SceneManager::EncodeComponentId(UINT32 idx, UINT32 type)
    {
        assert(idx <= (0x3FFFFFFF));

        return (type << 30) | idx;
    }

    void SceneManager::DecodeComponentId(UINT32 id, UINT32& idx, UINT32& type)
    {
        idx = id & 0x3FFFFFFF;
        type = id >> 30;
    }

    bool SceneManager::IsComponentOfType(const HComponent& component, UINT32 id)
    {
        return component->GetCoreType() == id;
    }

    void SceneManager::_update()
    {
        ProcessStateChanges();

        // Note: Eventually perform updates based on component types and/or on component priority. Right now we just
        // iterate in an undefined order, but it wouldn't be hard to change it.

        ScopeToggle toggle(_disableStateChange);
        for (auto& entry : _activeComponents)
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
