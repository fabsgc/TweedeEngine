#include "Scene/TeSceneObject.h"
#include "Scene/TeComponent.h"
#include "Scene/TeSceneManager.h"
#include "Scene/TeGameObjectManager.h"
#include "Math/TeMatrix3.h"
#include "TeCoreApplication.h"
#include "String/TeString.h"

namespace te
{
    SceneObject::SceneObject(const String& name, UINT32 flags)
        : GameObject(), _flags(flags)
    {
        SetName(name);
    }

    SceneObject::~SceneObject()
    {
        if (!_thisHandle.IsDestroyed())
        {
            TE_DEBUG("Object is being deleted without being destroyed first? {" + _name + "}", __FILE__, __LINE__);
            DestroyInternal(_thisHandle, true);
        }
    }

    HSceneObject SceneObject::Create(const String& name, UINT32 flags)
    {
        HSceneObject newObject = CreateInternal(name, flags);

        if (newObject->IsInstantiated())
            gSceneManager().RegisterNewSO(newObject);

        return newObject;
    }

    void SceneObject::Destroy(bool immediate)
    {
        // Parent is our owner, so when his reference to us is removed, delete might be called.
        // So make sure this is the last thing we do.
        if (_parent != nullptr)
        {
            if (!_parent.IsDestroyed())
                _parent->RemoveChild(_thisHandle);

            _parent = nullptr;
        }

        DestroyInternal(_thisHandle, immediate);
    }

    void SceneObject::_setFlags(UINT32 flags)
    {
        _flags |= flags;

        for (auto& child : _children)
            child->_setFlags(flags);
    }

    void SceneObject::_unsetFlags(UINT32 flags)
    {
        _flags &= ~flags;

        for (auto& child : _children)
            child->_unsetFlags(flags);
    }

    bool SceneObject::HasFlag(UINT32 flag) const
    {
        return (_flags & flag) != 0;
    }

    void SceneObject::_setInstanceData(GameObjectInstanceDataPtr& other)
    {
        GameObject::_setInstanceData(other);

        // Instance data changed, so make sure to refresh the handles to reflect that
        SPtr<SceneObject> thisPtr = _thisHandle.GetInternalPtr();
        _thisHandle._setHandleData(thisPtr);
    }

    void SceneObject::_instantiate()
    {
        std::function<void(SceneObject*)> instantiateRecursive = [&](SceneObject* obj)
        {
            obj->_flags &= ~SOF_DontInstantiate;

            if (obj->_parent == nullptr)
                gSceneManager().RegisterNewSO(obj->_thisHandle);

            for (auto& component : obj->_components)
                component->_instantiate();

            for (auto& child : obj->_children)
            {
                instantiateRecursive(child.Get());
            }
        };

        std::function<void(SceneObject*)> triggerEventsRecursive = [&](SceneObject* obj)
        {
            for (auto& component : obj->_components)
                gSceneManager()._notifyComponentCreated(component, obj->GetActive());

            for (auto& child : obj->_children)
            {
                triggerEventsRecursive(child.Get());
            }
        };

        instantiateRecursive(this);
        triggerEventsRecursive(this);
    }

    HSceneObject SceneObject::CreateInternal(const String& name, UINT32 flags)
    {
        SPtr<SceneObject> sceneObjectPtr = SPtr<SceneObject>(new (te_allocate<SceneObject>()) SceneObject(name, flags),
            &te_delete<SceneObject>);
        sceneObjectPtr->_UUID = UUIDGenerator::GenerateRandom();

        HSceneObject sceneObject = static_object_cast<SceneObject>(
            GameObjectManager::Instance().RegisterObject(sceneObjectPtr));
        sceneObject->_thisHandle = sceneObject;

        return sceneObject;
    }

    HSceneObject SceneObject::CreateInternal(const SPtr<SceneObject>& soPtr)
    {
        HSceneObject sceneObject = static_object_cast<SceneObject>(
            GameObjectManager::Instance().RegisterObject(soPtr));
        sceneObject->_thisHandle = sceneObject;

        return sceneObject;
    }

    void SceneObject::DestroyInternal(GameObjectHandleBase& handle, bool immediate)
    {
        if (immediate)
        {
            for (auto iter = _children.begin(); iter != _children.end(); ++iter)
                (*iter)->DestroyInternal(*iter, true);

            _children.clear();

            // It's important to remove the elements from the array as soon as they're destroyed, as OnDestroy callbacks
            // for components might query the SO's components, and we want to only return live ones
            while (!_components.empty())
            {
                HComponent component = _components.back();
                component->_setIsDestroyed();

                if (IsInstantiated())
                    gSceneManager()._notifyComponentDestroyed(component, immediate);

                component->DestroyInternal(component, true);
                _components.erase(_components.end() - 1);
            }

            GameObjectManager::Instance().UnregisterObject(handle);
        }
        else
        {
            GameObjectManager::Instance().QueueForDestroy(handle);
        }
    }

    const Transform& SceneObject::GetTransform() const
    {
        if (!IsCachedWorldTfrmUpToDate())
            UpdateWorldTfrm();

        return _worldTfrm;
    }

    void SceneObject::SetPosition(const Vector3& position)
    {
        if (_mobility == ObjectMobility::Movable)
        {
            _localTfrm.SetPosition(position);
            NotifyTransformChanged(TCF_Transform);
        }
    }

    void SceneObject::SetRotation(const Quaternion& rotation)
    {
        if (_mobility == ObjectMobility::Movable)
        {
            _localTfrm.SetRotation(rotation);
            NotifyTransformChanged(TCF_Transform);
        }
    }

    void SceneObject::SetScale(const Vector3& scale)
    {
        if (_mobility == ObjectMobility::Movable)
        {
            _localTfrm.SetScale(scale);
            NotifyTransformChanged(TCF_Transform);
        }
    }

    void SceneObject::SetWorldPosition(const Vector3& position)
    {
        if (_mobility != ObjectMobility::Movable)
            return;

        if (_parent != nullptr)
            _localTfrm.SetWorldPosition(position, _parent->GetTransform());
        else
            _localTfrm.SetPosition(position);

        NotifyTransformChanged(TCF_Transform);
    }

    void SceneObject::SetWorldRotation(const Quaternion& rotation)
    {
        if (_mobility != ObjectMobility::Movable)
            return;

        if (_parent != nullptr)
            _localTfrm.SetWorldRotation(rotation, _parent->GetTransform());
        else
            _localTfrm.SetRotation(rotation);

        NotifyTransformChanged(TCF_Transform);
    }

    void SceneObject::SetWorldScale(const Vector3& scale)
    {
        if (_mobility != ObjectMobility::Movable)
            return;

        if (_parent != nullptr)
            _localTfrm.SetWorldScale(scale, _parent->GetTransform());
        else
            _localTfrm.SetScale(scale);

        NotifyTransformChanged(TCF_Transform);
    }

    void SceneObject::LookAt(const Vector3& location, const Vector3& up)
    {
        const Transform& worldTfrm = GetTransform();

        Vector3 forward = location - worldTfrm.GetPosition();

        Quaternion rotation = worldTfrm.GetRotation();
        rotation.LookRotation(forward, up);
        SetWorldRotation(rotation);
    }

    const Matrix4& SceneObject::GetWorldMatrix() const
    {
        if (!IsCachedWorldTfrmUpToDate())
            UpdateWorldTfrm();

        return _cachedWorldTfrm;
    }

    Matrix4 SceneObject::GetInvWorldMatrix() const
    {
        if (!IsCachedWorldTfrmUpToDate())
            UpdateWorldTfrm();

        Matrix4 worldToLocal = _worldTfrm.GetInvMatrix();
        return worldToLocal;
    }

    const Matrix4& SceneObject::GetLocalMatrix() const
    {
        if (!IsCachedLocalTfrmUpToDate())
            UpdateLocalTfrm();

        return _cachedLocalTfrm;
    }

    void SceneObject::Move(const Vector3& vec)
    {
        if (_mobility == ObjectMobility::Movable)
        {
            _localTfrm.Move(vec);
            NotifyTransformChanged(TCF_Transform);
        }
    }

    void SceneObject::MoveRelative(const Vector3& vec)
    {
        if (_mobility == ObjectMobility::Movable)
        {
            _localTfrm.MoveRelative(vec);
            NotifyTransformChanged(TCF_Transform);
        }
    }

    void SceneObject::Rotate(const Vector3& axis, const Radian& angle)
    {
        if (_mobility == ObjectMobility::Movable)
        {
            _localTfrm.Rotate(axis, angle);
            NotifyTransformChanged(TCF_Transform);
        }
    }

    void SceneObject::Rotate(const Quaternion& q)
    {
        if (_mobility == ObjectMobility::Movable)
        {
            _localTfrm.Rotate(q);
            NotifyTransformChanged(TCF_Transform);
        }
    }

    void SceneObject::Roll(const Radian& angle)
    {
        if (_mobility == ObjectMobility::Movable)
        {
            _localTfrm.Roll(angle);
            NotifyTransformChanged(TCF_Transform);
        }
    }

    void SceneObject::Yaw(const Radian& angle)
    {
        if (_mobility == ObjectMobility::Movable)
        {
            _localTfrm.Yaw(angle);
            NotifyTransformChanged(TCF_Transform);
        }
    }

    void SceneObject::Pitch(const Radian& angle)
    {
        if (_mobility == ObjectMobility::Movable)
        {
            _localTfrm.Pitch(angle);
            NotifyTransformChanged(TCF_Transform);
        }
    }

    void SceneObject::SetForward(const Vector3& forwardDir)
    {
        const Transform& worldTfrm = GetTransform();

        Quaternion currentRotation = worldTfrm.GetRotation();
        currentRotation.LookRotation(forwardDir);
        SetWorldRotation(currentRotation);
    }

    void SceneObject::UpdateTransformsIfDirty()
    {
        if (!IsCachedLocalTfrmUpToDate())
            UpdateLocalTfrm();

        if (!IsCachedWorldTfrmUpToDate())
            UpdateWorldTfrm();
    }

    void SceneObject::DestroyComponent(const HComponent component, bool immediate)
    {
        if (component == nullptr)
        {
            TE_DEBUG("Trying to remove a null component", __FILE__, __LINE__);
            return;
        }

        auto iter = std::find(_components.begin(), _components.end(), component);

        if (iter != _components.end())
        {
            (*iter)->_setIsDestroyed();

            if (IsInstantiated())
                gSceneManager()._notifyComponentDestroyed(*iter, immediate);

            (*iter)->DestroyInternal(*iter, immediate);
            _components.erase(iter);
        }
        else
            TE_DEBUG("Trying to remove a component that doesn't exist on this SceneObject.", __FILE__, __LINE__);
    }

    void SceneObject::DestroyComponent(Component* component, bool immediate)
    {
        auto iterFind = std::find_if(_components.begin(), _components.end(),
            [component](const HComponent& x)
            {
                if (x.IsDestroyed())
                    return false;

                return x._getHandleData()->Ptr->Object.get() == component; }
        );

        if (iterFind != _components.end())
        {
            DestroyComponent(*iterFind, immediate);
        }
    }

    void SceneObject::NotifyTransformChanged(TransformChangedFlags flags) const
    {
        // If object is immovable, don't send transform changed events nor mark the transform dirty
        TransformChangedFlags componentFlags = flags;
        if (_mobility != ObjectMobility::Movable)
            componentFlags = (TransformChangedFlags)(componentFlags & ~TCF_Transform);
        else
        {
            _dirtyFlags |= DirtyFlags::LocalTfrmDirty | DirtyFlags::WorldTfrmDirty;
            _dirtyHash++;
        }

        // Only send component flags if we haven't removed them all
        if (componentFlags != 0)
        {
            for (auto& entry : _components)
            {
                if (entry->SupportsNotify(flags))
                {
                    bool alwaysRun = entry->HasFlag((UINT32)ComponentFlag::AlwaysRun);
                    if (alwaysRun || gSceneManager().IsRunning())
                        entry->OnTransformChanged(componentFlags);
                }
            }
        }

        // Mobility flag is only relevant for this scene object
        flags = (TransformChangedFlags)(flags & ~TCF_Mobility);
        if (flags != 0)
        {
            for (auto& entry : _children)
                entry->NotifyTransformChanged(flags);
        }
    }

    void SceneObject::UpdateWorldTfrm() const
    {
        _worldTfrm = _localTfrm;

        // Don't allow movement from parent when not movable
        if (_parent != nullptr && _mobility == ObjectMobility::Movable)
        {
            _worldTfrm.MakeWorld(_parent->GetTransform());

            _cachedWorldTfrm = _worldTfrm.GetMatrix();
        }
        else
        {
            _cachedWorldTfrm = GetLocalMatrix();
        }

        _dirtyFlags &= ~DirtyFlags::WorldTfrmDirty;
    }

    void SceneObject::UpdateLocalTfrm() const
    {
        _cachedLocalTfrm = _localTfrm.GetMatrix();
        _dirtyFlags &= ~DirtyFlags::LocalTfrmDirty;
    }

    void SceneObject::SetParent(const HSceneObject& parent, bool keepWorldTransform)
    {
        if (parent.IsDestroyed())
            return;

        if (_mobility != ObjectMobility::Movable)
            keepWorldTransform = true;

        _setParent(parent, keepWorldTransform);
    }

    HSceneObject SceneObject::GetChild(UINT32 idx) const
    {
        if (idx >= _children.size())
        {
            TE_ASSERT_ERROR(false, "Child index out of range.", __FILE__, __LINE__);
        }

        return _children[idx];
    }

    int SceneObject::IndexOfChild(const HSceneObject& child) const
    {
        for (int i = 0; i < (int)_children.size(); i++)
        {
            if (_children[i] == child)
                return i;
        }

        return -1;
    }

    const SPtr<SceneInstance>& SceneObject::GetScene() const
    {
        if (_parentScene)
            return _parentScene;

        TE_DEBUG("Attempting to access a scene of a SceneObject with no scene, returning main scene instead.", __FILE__, __LINE__);
        return gSceneManager().GetMainScene();
    }

    void SceneObject::SetScene(const SPtr<SceneInstance>& scene)
    {
        if (_parentScene == scene)
            return;

        _parentScene = scene;

        for (auto& child : _children)
            child->SetScene(scene);
    }

    void SceneObject::SetActive(bool active)
    {
        _activeSelf = active;
        SetActiveHierarchy(active);
    }

    bool SceneObject::GetActive(bool self) const
    {
        if (self)
            return _activeSelf;
        else
            return _activeHierarchy;
    }

    void SceneObject::SetActiveHierarchy(bool active, bool triggerEvents)
    {
        bool activeHierarchy = active && _activeSelf;

        if (_activeHierarchy != activeHierarchy)
        {
            _activeHierarchy = activeHierarchy;

            if (triggerEvents)
            {
                if (activeHierarchy)
                {
                    for (auto& component : _components)
                        gSceneManager()._notifyComponentActivated(component, triggerEvents);
                }
                else
                {
                    for (auto& component : _components)
                        gSceneManager()._notifyComponentDeactivated(component, triggerEvents);
                }
            }
        }

        for (auto child : _children)
        {
            child->SetActiveHierarchy(_activeHierarchy, triggerEvents);
        }
    }

    void SceneObject::SetMobility(ObjectMobility mobility)
    {
        if (_mobility != mobility)
        {
            _mobility = mobility;

            // If mobility changed to movable, update both the mobility flag and transform, otherwise just mobility
            if (_mobility == ObjectMobility::Movable)
                NotifyTransformChanged((TransformChangedFlags)(TCF_Transform | TCF_Mobility));
            else
                NotifyTransformChanged(TCF_Mobility);
        }
    }

    void SceneObject::_setParent(const HSceneObject& parent, bool keepWorldTransform)
    {
        if (_thisHandle == parent)
            return;

        if (_parent == nullptr || _parent != parent)
        {
            Transform worldTfrm;

            // Make sure the object keeps its world coordinates
            if (keepWorldTransform)
                worldTfrm = GetTransform();

            if (_parent != nullptr)
                _parent->RemoveChild(_thisHandle);

            if (parent != nullptr)
            {
                parent->AddChild(_thisHandle);
                SetScene(parent->_parentScene);
            }
            else
                SetScene(nullptr);

            _parent = parent;

            if (keepWorldTransform)
            {
                _localTfrm = worldTfrm;

                if (_parent != nullptr)
                    _localTfrm.MakeLocal(_parent->GetTransform());
            }

            bool isInstantiated = (_flags & SOF_DontInstantiate) == 0;
            if (isInstantiated)
                NotifyTransformChanged((TransformChangedFlags)(TCF_Parent | TCF_Transform));
        }
    }

    void SceneObject::AddChild(const HSceneObject& object)
    {
        _children.push_back(object);

        object->_setFlags(_flags);
    }

    void SceneObject::RemoveChild(const HSceneObject& object)
    {
        auto result = find(_children.begin(), _children.end(), object);

        if (result != _children.end())
            _children.erase(result);
        else
        {
            TE_ASSERT_ERROR(false, "Trying to remove a child but it's not a child of the transform.", __FILE__, __LINE__);
        }
    }

    HComponent SceneObject::GetComponent(UINT32 type) const
    {
        if (type != TID_Component)
        {
            for (auto& entry : _components)
            {
                if (entry->GetCoreType() == type)
                    return entry;
            }
        }

        return HComponent();
    }

    void SceneObject::AddComponentInternal(const SPtr<Component>& component)
    {
        HComponent newComponent = static_object_cast<Component>(
            GameObjectManager::Instance().GetObjectHandle(component->GetInstanceId()));
        newComponent->_parent = _thisHandle;
        newComponent->_thisHandle = newComponent;

        _components.push_back(newComponent);
    }

    void SceneObject::AddAndInitializeComponent(const HComponent& component)
    {
        component->_thisHandle = component;

        if (component->_UUID.Empty())
            component->_UUID = UUIDGenerator::GenerateRandom();

        _components.push_back(component);

        if (IsInstantiated())
        {
            component->_instantiate();

            gSceneManager()._notifyComponentCreated(component, GetActive());
        }
    }

    void SceneObject::AddAndInitializeComponent(const SPtr<Component>& component)
    {
        HComponent newComponent = static_object_cast<Component>(
            GameObjectManager::Instance().GetObjectHandle(component->GetInstanceId()));
        newComponent->_parent = _thisHandle;

        AddAndInitializeComponent(newComponent);
    }
}
