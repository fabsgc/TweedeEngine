#include "Scene/TeSceneObject.h"
#include "Scene/TeComponent.h"
#include "Scene/TeSceneManager.h"
#include "Scene/TeGameObjectManager.h"
#include "Math/TeMatrix3.h"
#include "TeCoreApplication.h"
#include "String/TeString.h"
#include "Image/TeColor.h"

#include "Components/TeCCamera.h"
#include "Components/TeCCameraFlyer.h"
#include "Components/TeCCameraUI.h"
#include "Components/TeCLight.h"
#include "Components/TeCRenderable.h"
#include "Components/TeCSkybox.h"
#include "Components/TeCScript.h"
#include "Components/TeCAnimation.h"
#include "Components/TeCBone.h"
#include "Components/TeCAudioSource.h"
#include "Components/TeCAudioListener.h"
#include "Components/TeCRigidBody.h"
#include "Components/TeCMeshSoftBody.h"
#include "Components/TeCEllipsoidSoftBody.h"
#include "Components/TeCRopeSoftBody.h"
#include "Components/TeCPatchSoftBody.h"
#include "Components/TeCConeTwistJoint.h"
#include "Components/TeCD6Joint.h"
#include "Components/TeCHingeJoint.h"
#include "Components/TeCSliderJoint.h"
#include "Components/TeCSphericalJoint.h"
#include "Components/TeCBoxCollider.h"
#include "Components/TeCPlaneCollider.h"
#include "Components/TeCSphereCollider.h"
#include "Components/TeCCylinderCollider.h"
#include "Components/TeCCapsuleCollider.h"
#include "Components/TeCMeshCollider.h"
#include "Components/TeCConeCollider.h"
#include "Components/TeCAnimation.h"

namespace te
{
    SceneObject::SceneObject(const String& name, UINT32 flags)
        : GameObject()
        , Serializable(TID_SceneObject)
        , _flags(flags)
    {
        SetName(name);
    }

    SceneObject::~SceneObject()
    {
        if (!_thisHandle.IsDestroyed())
        {
            TE_DEBUG("Object is being deleted without being destroyed first? {" + _name + "}");
            DestroyInternal(_thisHandle, true);
        }
    }

    HSceneObject SceneObject::Create(const String& name, UINT32 flags)
    {
        HSceneObject newObject = CreateInternal(name, flags);
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

    bool SceneObject::Clone(const HSceneObject so, const String& suffix)
    {
        if (so.Empty())
        {
            TE_DEBUG("Tries to clone a sceneObject using an invalid sceneObject handle");
            return false;
        }

        return Clone(so.GetInternalPtr());
    }

    bool SceneObject::Clone(const SPtr<SceneObject>& so, const String& suffix)
    {
        if (!so)
        {
            TE_DEBUG("Tries to clone a sceneObject using an invalid sceneObject pointer");
            return false;
        }

        Transform tfrm = so->GetTransform();
        this->_name = so->GetName() + " copy";
        this->_mobility = so->GetMobility();
        this->SetLocalTransform(tfrm);

        for (auto& childSO : so->GetChildren())
        {
            // Create a copy of each children
            // Add it to the list of children

            HSceneObject sceneObject = SceneObject::Create("SceneObject");
            sceneObject->Clone(childSO);
            sceneObject->SetParent(this->GetHandle());
        }

        for (auto& co : so->GetComponents())
        {
            // Create a copy a each component
            // Add it to the list of components

            switch (co->GetCoreType())
            {
            case TID_CCamera:
            {
                HCamera component = this->AddComponent<CCamera>();
                component->Clone(co->GetHandle());
                component->Initialize();
            }
            break;

            case TID_CCameraFlyer:
            {
                HCameraFlyer component = this->AddComponent<CCameraFlyer>();
                component->Clone(co->GetHandle());
                component->Initialize();
            }
            break;

            case TID_CCameraUI:
            {
                HCameraUI component = this->AddComponent<CCameraUI>();
                component->Clone(co->GetHandle());
                component->Initialize();
            }
            break;

            case TID_CLight:
            {
                HLight previousLight = static_object_cast<CLight>(co);
                HLight component = this->AddComponent<CLight>(previousLight->GetType());
                component->Clone(co->GetHandle());
                component->Initialize();
            }
            break;

            case TID_CRenderable:
            {
                HRenderable component = this->AddComponent<CRenderable>();
                component->Clone(co->GetHandle());
                component->Initialize();
            }
            break;

            case TID_CScript:
            {
                HScript component = this->AddComponent<CScript>();
                component->Clone(co->GetHandle());
                component->Initialize();
            }
            break;

            case TID_CSkybox:
            {
                if (SceneManager::Instance().FindComponents<CSkybox>().size() > 0)
                    break;

                HSkybox component = this->AddComponent<CSkybox>();
                component->Clone(co->GetHandle());
                component->Initialize();
            }
            break;

            case TID_CAnimation:
            {
                HAnimation component = this->AddComponent<CAnimation>();
                component->Clone(co->GetHandle());
                component->Initialize();
            }
            break;

            case TID_CBone:
            {
                HBone component = this->AddComponent<CBone>();
                component->Clone(co->GetHandle());
                component->Initialize();
            }
            break;

            case TID_CAudioListener:
            {
                HAudioListener component = this->AddComponent<CAudioListener>();
                component->Clone(co->GetHandle());
                component->Initialize();
            }
            break;

            case TID_CAudioSource:
            {
                HAudioSource component = this->AddComponent<CAudioSource>();
                component->Clone(co->GetHandle());
                component->Initialize();
            }
            break;

            case TID_CRigidBody:
            {
                HRigidBody component = this->AddComponent<CRigidBody>();
                component->Clone(co->GetHandle());
                component->Initialize();
            }
            break;

            case TID_CMeshSoftBody:
            {
                HMeshSoftBody component = this->AddComponent<CMeshSoftBody>();
                component->Clone(co->GetHandle());
                component->Initialize();
            }
            break;

            case TID_CEllipsoidSoftBody:
            {
                /* HEllipsoidSoftBody component = this->AddComponent<CEllipsoidSoftBody>();
                component->Clone(co->GetHandle());
                component->Initialize(); // TODO */
            }
            break;

            case TID_CRopeSoftBody:
            {
                /* HRopeSoftBody component = this->AddComponent<CRopeSoftBody>();
                component->Clone(co->GetHandle());
                component->Initialize(); // TODO */
            }
            break;

            case TID_CPatchSoftBody:
            {
                /* HPatchSoftBody component = this->AddComponent<CPatchSoftBody>();
                component->Clone(co->GetHandle());
                component->Initialize(); // TODO */
            }
            break;

            case TID_CConeTwistJoint:
            {
                HConeTwistJoint component = this->AddComponent<CConeTwistJoint>();
                component->Clone(co->GetHandle());
                component->Initialize();
            }
            break;

            case TID_CD6Joint:
            {
                HD6Joint component = this->AddComponent<CD6Joint>();
                component->Clone(co->GetHandle());
                component->Initialize();
            }
            break;

            case TID_CHingeJoint:
            {
                HHingeJoint component = this->AddComponent<CHingeJoint>();
                component->Clone(co->GetHandle());
                component->Initialize();
            }
            break;

            case TID_CSliderJoint:
            {
                HSliderJoint component = this->AddComponent<CSliderJoint>();
                component->Clone(co->GetHandle());
                component->Initialize();
            }
            break;

            case TID_CSphericalJoint:
            {
                HSphericalJoint component = this->AddComponent<CSphericalJoint>();
                component->Clone(co->GetHandle());
                component->Initialize();
            }
            break;

            case TID_CBoxCollider:
            {
                HBoxCollider component = this->AddComponent<CBoxCollider>();
                component->Clone(co->GetHandle());
                component->Initialize();
            }
            break;

            case TID_CCapsuleCollider:
            {
                HCapsuleCollider component = this->AddComponent<CCapsuleCollider>();
                component->Clone(co->GetHandle());
                component->Initialize();
            }
            break;

            case TID_CConeCollider:
            {
                HConeCollider component = this->AddComponent<CConeCollider>();
                component->Clone(co->GetHandle());
                component->Initialize();
            }
            break;

            case TID_CCylinderCollider:
            {
                HCylinderCollider component = this->AddComponent<CCylinderCollider>();
                component->Clone(co->GetHandle());
                component->Initialize();
            }
            break;

            case TID_CHeightFieldCollider:
            {
                HHeightFieldCollider component = this->AddComponent<CHeightFieldCollider>();
                component->Clone(co->GetHandle());
                component->Initialize();
            }
            break;

            case TID_CMeshCollider:
            {
                HMeshCollider component = this->AddComponent<CMeshCollider>();
                component->Clone(co->GetHandle());
                component->Initialize();
            }
            break;

            case TID_CPlaneCollider:
            {
                HPlaneCollider component = this->AddComponent<CPlaneCollider>();
                component->Clone(co->GetHandle());
                component->Initialize();
                component->SetName(co->GetName());
            }
            break;

            case TID_CSphereCollider:
            {
                HSphereCollider component = this->AddComponent<CSphereCollider>();
                component->Clone(co->GetHandle());
                component->Initialize();
            }
            break;

            default:
                break;
            }
        }

        return true;
    }

    void SceneObject::SetInstanceData(GameObjectInstanceDataPtr& other)
    {
        GameObject::SetInstanceData(other);

        // Instance data changed, so make sure to refresh the handles to reflect that
        SPtr<SceneObject> thisPtr = _thisHandle.GetInternalPtr();
        _thisHandle = thisPtr;
    }

    HSceneObject SceneObject::CreateInternal(const String& name, UINT32 flags)
    {
        SPtr<SceneObject> sceneObjectPtr = SPtr<SceneObject>(new (te_allocate<SceneObject>()) SceneObject(name, flags),
            &te_delete<SceneObject>);

        sceneObjectPtr->SetUUID(UUIDGenerator::GenerateRandom());
        sceneObjectPtr->SetColor(Color::GenerateRandom(0.1f, 1.0f));

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
                component->SetIsDestroyed();

                gSceneManager().NotifyComponentDestroyed(component, immediate);

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

    void SceneObject::SetLocalTransform(Transform& tfrm)
    { 
        if (_mobility == ObjectMobility::Movable)
        {
            _localTfrm = tfrm;
            NotifyTransformChanged(TCF_Transform);
        }
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

    void SceneObject::RotateAround(const Vector3& center, const Vector3& axis, const Radian& angle)
    {
        if (_mobility == ObjectMobility::Movable)
        {
            _localTfrm.RotateAround(center, axis, angle);
            NotifyTransformChanged(TCF_Transform);
        }
    }

    void SceneObject::RotateAround(const Vector3& center, const Quaternion& rotation)
    {
        if (_mobility == ObjectMobility::Movable)
        {
            _localTfrm.RotateAround(center, rotation);
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
            TE_DEBUG("Trying to remove a null component");
            return;
        }

        auto iter = std::find(_components.begin(), _components.end(), component);

        if (iter != _components.end())
        {
            (*iter)->SetIsDestroyed();

            gSceneManager().NotifyComponentDestroyed(*iter, immediate);

            (*iter)->DestroyInternal(*iter, immediate);
            _components.erase(iter);
        }
        else
        {
            TE_DEBUG("Trying to remove a component that doesn't exist on this SceneObject.");
        }
    }

    void SceneObject::DestroyComponent(Component* component, bool immediate)
    {
        auto iterFind = std::find_if(_components.begin(), _components.end(),
            [component](const HComponent& x) {
                if (x.IsDestroyed())
                    return false;

                if (!x.GetHandleData()->Ptr && !component)
                    return true;

                return x.GetHandleData()->Ptr->Object.get() == component; 
            }
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
                    bool alwaysRun = entry->HasFlag(Component::AlwaysRun);
                    bool isRunning = gCoreApplication().GetState().IsFlagSet(ApplicationState::Game);

                    if (alwaysRun || isRunning)
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
            TE_ASSERT_ERROR(false, "Child index out of range.");
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

        TE_DEBUG("Attempting to access a scene of a SceneObject with no scene, returning main scene instead.");
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
                        gSceneManager().NotifyComponentActivated(component, triggerEvents);
                }
                else
                {
                    for (auto& component : _components)
                        gSceneManager().NotifyComponentDeactivated(component, triggerEvents);
                }
            }
        }

        for (auto child : _children)
        {
            child->SetActiveHierarchy(_activeHierarchy, triggerEvents);
        }
    }

    bool SceneObject::GetActive(bool self) const
    {
        if (self)
            return _activeSelf;
        else
            return _activeHierarchy;
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
            {
                SetScene(nullptr);
            }

            _parent = parent;

            if (keepWorldTransform)
            {
                _localTfrm = worldTfrm;

                if (_parent != nullptr)
                    _localTfrm.MakeLocal(_parent->GetTransform());
            }

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
            TE_ASSERT_ERROR(false, "Trying to remove a child but it's not a child of the transform.");
        }
    }

    HComponent SceneObject::GetComponent(UINT32 type, bool searchInChildren) const
    {
        if (type == TID_Component)
            return HComponent();

        return _getComponentInternal(_thisHandle, type, ComponentSearchType::CoreType, searchInChildren);
    }

    Vector<HComponent> SceneObject::GetComponents(UINT32 type, bool searchInChildren) const
    {
        if (type == TID_Component)
            return Vector<HComponent>();

        Vector<HComponent> components;
        _getComponentsInternal(_thisHandle, type, components, ComponentSearchType::CoreType, searchInChildren);

        return components;
    }

    Vector<HComponent> SceneObject::GetComponents(Vector<UINT32> types, bool searchInChildren) const
    {
        Vector<HComponent> components;

        for (auto& type : types)
        {
            if (type == TID_Component)
                continue;

            _getComponentsInternal(_thisHandle, type, components, ComponentSearchType::CoreType, searchInChildren);
        }

        return components;
    }

    HComponent SceneObject::GetComponent(const String& name, bool searchInChildren) const
    {
        return _getComponentInternal(_thisHandle, name, ComponentSearchType::Name, searchInChildren);
    }

    Vector<HComponent> SceneObject::GetComponents(const String& name, bool searchInChildren) const
    {
        Vector<HComponent> components;
        _getComponentsInternal(_thisHandle, name, components, ComponentSearchType::Name, searchInChildren);

        return components;
    }

    HComponent SceneObject::GetComponent(const UUID& uuid, bool searchInChildren) const
    {
        return _getComponentInternal(_thisHandle, uuid, ComponentSearchType::UUID, searchInChildren);
    }

    Vector<HComponent> SceneObject::GetComponents(const UUID& uuid, bool searchInChildren) const
    {
        Vector<HComponent> components;
        _getComponentsInternal(_thisHandle, uuid, components, ComponentSearchType::UUID, searchInChildren);

        return components;
    }

    HComponent SceneObject::_getComponentInternal(const HSceneObject& currentSO, std::any criteria, 
        ComponentSearchType searchType, bool searchInChildren) const
    {
        HComponent component;

        for (auto& entry : currentSO->GetComponents())
        {
            if (searchType == ComponentSearchType::CoreType)
            {
                if (entry->GetCoreType() == std::any_cast<UINT32>(criteria))
                {
                    component = entry.GetNewHandleFromExisting();
                    break;
                }
            }
            else if (searchType == ComponentSearchType::Name)
            {
                if (entry->GetName() == std::any_cast<String>(criteria))
                {
                    component = entry.GetNewHandleFromExisting();
                    break;
                }
            }
            else if (searchType == ComponentSearchType::UUID)
            {
                if (entry->GetUUID() == std::any_cast<UUID>(criteria))
                {
                    component = entry.GetNewHandleFromExisting();
                    break;
                }
            }
            else
                break;
        }

        if (component.Empty() && searchInChildren)
        {
            for (auto& childSO : currentSO->GetChildren())
            {
                component = _getComponentInternal(childSO, criteria, searchType, searchInChildren);
                if (!component.Empty())
                    break;
            }
        }

        return component;
    }

    void SceneObject::_getComponentsInternal(const HSceneObject& currentSO, std::any criteria, Vector<HComponent>& components, 
        ComponentSearchType searchType, bool searchInChildren) const
    {
        for (auto& entry : currentSO->GetComponents())
        {
            if (searchType == ComponentSearchType::CoreType)
            {
                if (entry->GetCoreType() == std::any_cast<UINT32>(criteria))
                {
                    components.push_back(entry.GetNewHandleFromExisting());
                    break;
                }
            }
            else if (searchType == ComponentSearchType::Name)
            {
                if (entry->GetName() == std::any_cast<String>(criteria))
                {
                    components.push_back(entry.GetNewHandleFromExisting());
                    break;
                }
            }
            else if (searchType == ComponentSearchType::UUID)
            {
                if (entry->GetUUID() == std::any_cast<UUID>(criteria))
                {
                    components.push_back(entry.GetNewHandleFromExisting());
                    break;
                }
            }
        }

        if (searchInChildren)
        {
            for (auto& childSO : currentSO->GetChildren())
                _getComponentsInternal(childSO, criteria, components, searchType, searchInChildren);
        }
    }

    HSceneObject SceneObject::GetSceneObject(const String& name, bool searchInChildren) const
    {
        return _getSceneObjectInternal(_thisHandle, name, ComponentSearchType::Name, searchInChildren);
    }

    Vector<HSceneObject> SceneObject::GetSceneObjects(const String& name, bool searchInChildren) const
    {
        Vector<HSceneObject> sceneObjects;
        _getSceneObjectsInternal(_thisHandle, name, sceneObjects, ComponentSearchType::Name, searchInChildren);

        return sceneObjects;
    }

    HSceneObject SceneObject::GetSceneObject(const UUID& uuid, bool searchInChildren) const
    {
        return _getSceneObjectInternal(_thisHandle, uuid, ComponentSearchType::UUID, searchInChildren);
    }

    Vector<HSceneObject> SceneObject::GetSceneObjects(const UUID& uuid, bool searchInChildren) const
    {
        Vector<HSceneObject> sceneObjects;
        _getSceneObjectsInternal(_thisHandle, uuid, sceneObjects, ComponentSearchType::UUID, searchInChildren);

        return sceneObjects;
    }

    HSceneObject SceneObject::_getSceneObjectInternal(const HSceneObject& currentSO, std::any criteria, 
        ComponentSearchType searchType, bool searchInChildren) const
    {
        HSceneObject sceneObject;

        for (auto& entry : currentSO->GetChildren())
        {
            if (searchType == ComponentSearchType::Name)
            {
                if (entry->GetName() == std::any_cast<String>(criteria))
                {
                    sceneObject = entry.GetNewHandleFromExisting();
                    break;
                }
            }
            else if (searchType == ComponentSearchType::UUID)
            {
                if (entry->GetUUID() == std::any_cast<UUID>(criteria))
                {
                    sceneObject = entry.GetNewHandleFromExisting();
                    break;
                }
            }
            else
                break;
        }

        if (sceneObject.Empty() && searchInChildren)
        {
            for (auto& childSO : currentSO->GetChildren())
            {
                sceneObject = _getSceneObjectInternal(childSO, criteria, searchType, searchInChildren);
                if (!sceneObject.Empty())
                    break;
            }
        }

        return sceneObject;
    }

    void SceneObject::_getSceneObjectsInternal(const HSceneObject& currentSO, std::any criteria, Vector<HSceneObject>& sceneObjects, 
        ComponentSearchType searchType, bool searchInChildren) const
    {
        for (auto& entry : currentSO->GetChildren())
        {
            if (searchType == ComponentSearchType::Name)
            {
                if (entry->GetName() == std::any_cast<String>(criteria))
                {
                    sceneObjects.push_back(entry.GetNewHandleFromExisting());
                    break;
                }
            }
            else if (searchType == ComponentSearchType::UUID)
            {
                if (entry->GetUUID() == std::any_cast<UUID>(criteria))
                {
                    sceneObjects.push_back(entry.GetNewHandleFromExisting());
                    break;
                }
            }
        }

        if (searchInChildren)
        {
            for (auto& childSO : currentSO->GetChildren())
                _getSceneObjectsInternal(childSO, criteria, sceneObjects, searchType, searchInChildren);
        }
    }

    void SceneObject::AddComponentInternal(const SPtr<Component>& component)
    {
        HComponent newComponent = static_object_cast<Component>(
            GameObjectManager::Instance().GetObjectHandle(component->GetInstanceId()));
        newComponent->_parent = _thisHandle;
        newComponent->_thisHandle = newComponent;

        _components.push_back(newComponent);
    }

    bool SceneObject::IsDescendantOf(const HSceneObject& sceneObject)
    {
        for (const auto& childSO : sceneObject->GetChildren())
        {
            if (GetUUID() == childSO->GetUUID())
                return true;

            if (childSO->GetNumChildren() > 0)
            {
                if (IsDescendantOf(childSO))
                    return true;
            }
        }

        return false;
    }

    void SceneObject::AddAndInitializeComponent(const HComponent& component)
    {
        component->_thisHandle = component;

        if (component->_UUID.Empty())
            component->_UUID = UUIDGenerator::GenerateRandom();

        component->_gameObjectColor = Color::GenerateRandom(0.1f, 1.0f);
        component->Instantiate();

        _components.push_back(component);

        gSceneManager().NotifyComponentCreated(component);
    }

    void SceneObject::AddAndInitializeComponent(const SPtr<Component>& component)
    {
        HComponent newComponent = static_object_cast<Component>(
            GameObjectManager::Instance().GetObjectHandle(component->GetInstanceId()));
        newComponent->_parent = _thisHandle;

        AddAndInitializeComponent(newComponent);
    }

    void SceneObject::RemoveComponent(const HComponent& component)
    {
        for (auto iter = _components.begin(); iter != _components.end(); iter++)
        {
            if ((*iter)->GetUUID() == component->GetUUID())
            {
                _components.erase(iter);
                break;
            }
        }
    }

    void SceneObject::AddExistingComponent(const HComponent& component)
    {
        bool alreadyPresent = false;
        for (auto iter = _components.begin(); iter != _components.end(); iter++)
        {
            if ((*iter)->GetUUID() == component->GetUUID())
            {
                alreadyPresent = true;
                break;
            }
        }

        if (!alreadyPresent)
            _components.push_back(component.GetNewHandleFromExisting());
    }
}
