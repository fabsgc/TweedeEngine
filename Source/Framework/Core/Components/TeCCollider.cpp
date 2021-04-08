#include "Components/TeCCollider.h"
#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"
#include "Components/TeCRigidBody.h"

using namespace std::placeholders;

namespace te
{
    CCollider::CCollider(UINT32 type)
        : Component(HSceneObject(), type)
        , _internal(nullptr)
    {
        SetName("Collider");
        _notifyFlags = (TransformChangedFlags)(TCF_Parent | TCF_Transform);
    }

    CCollider::CCollider(const HSceneObject& parent, UINT32 type)
        : Component(parent, type)
        , _internal(nullptr)
    {
        SetName("Collider");
        _notifyFlags = (TransformChangedFlags)(TCF_Parent | TCF_Transform);
    }

    CCollider::~CCollider()
    { }

    void CCollider::Initialize()
    { 
        RestoreInternal();
        Component::Initialize();
    }

    void CCollider::Clone(const HComponent& c)
    {
        Clone(static_object_cast<CCollider>(c));
    }

    void CCollider::Clone(const HCollider& c)
    {
        Component::Clone(c.GetInternalPtr());

        _collisionReportMode = c->_collisionReportMode;
        _isTrigger = c->_isTrigger;
        _mass = c->_mass;
        _localPosition = c->_localPosition;
        _localRotation = c->_localRotation;
    }

    void CCollider::SetIsTrigger(bool value)
    {
        if (_isTrigger == value)
            return;

        _isTrigger = value;

        if (_internal != nullptr)
        {
            _internal->SetIsTrigger(value);

            UpdateParentRigidbody();
            UpdateTransform();
        }
    }

    void CCollider::SetCollisionReportMode(CollisionReportMode mode)
    {
        _collisionReportMode = mode;

        if (_internal != nullptr)
            UpdateCollisionReportMode();
    }

    void CCollider::OnInitialized()
    { }

    void CCollider::OnDestroyed()
    {
        if (_internal != nullptr)
            DestroyInternal();
    }

    void CCollider::OnDisabled()
    {
        if (_internal != nullptr)
            DestroyInternal();
    }

    void CCollider::OnEnabled()
    {
        RestoreInternal();
    }

    void CCollider::OnTransformChanged(TransformChangedFlags flags)
    {
        if (_internal == nullptr)
            return;

        if ((flags & TCF_Parent) != 0)
            UpdateParentRigidbody();

        // Don't update the transform if it's due to Physics update since then we can guarantee it will remain at the same
        // relative transform to its parent
        if (gPhysics().IsUpdateInProgress())
            return;

        if ((flags & (TCF_Parent | TCF_Transform)) != 0)
            UpdateTransform();
    }

    void CCollider::RestoreInternal()
    {
        if (_internal == nullptr)
        {
            _internal = CreateInternal();

            _internal->OnCollisionBegin.Connect(std::bind(&CCollider::TriggerOnCollisionBegin, this, _1));
            _internal->OnCollisionStay.Connect(std::bind(&CCollider::TriggerOnCollisionStay, this, _1));
            _internal->OnCollisionEnd.Connect(std::bind(&CCollider::TriggerOnCollisionEnd, this, _1));
        }

        UpdateParentRigidbody();
        UpdateTransform();
        UpdateCollisionReportMode();
    }

    void CCollider::DestroyInternal()
    { 
        if (_parent != nullptr)
            _parent->RemoveCollider(static_object_cast<CCollider>(_thisHandle));

        _parent = nullptr;

        // This should release the last reference and destroy the internal collider
        if (_internal)
        {
            _internal->SetOwner(PhysicsOwnerType::None, nullptr);
            _internal = nullptr;
        }
    }

    void CCollider::SetRigidBody(const HRigidBody& rigidbody, bool internal)
    { 
        if (rigidbody == _parent)
            return;

        if (_internal != nullptr && !internal)
        {
            if (_parent != nullptr)
                _parent->RemoveCollider(static_object_cast<CCollider>(_thisHandle));

            RigidBody* rigidBodyPtr = nullptr;

            if (rigidbody != nullptr)
                rigidBodyPtr = rigidbody->GetInternal();

            _internal->SetRigidBody(rigidBodyPtr);

            if (rigidbody != nullptr)
                rigidbody->AddCollider(static_object_cast<CCollider>(_thisHandle));
        }

        _parent = rigidbody;
        UpdateCollisionReportMode();
        UpdateTransform();
    }

    void CCollider::UpdateTransform()
    {
        const Transform& tfrm = SO()->GetTransform();
        Vector3 myScale = tfrm.GetScale();

        if (_parent != nullptr)
        {
            const Transform& parentTfrm = _parent->SO()->GetTransform();
            Vector3 parentPos = parentTfrm.GetPosition();
            Quaternion parentRot = parentTfrm.GetRotation();

            Vector3 myPos = tfrm.GetPosition();
            Quaternion myRot = tfrm.GetRotation();

            Vector3 scale = parentTfrm.GetScale();
            Vector3 invScale = scale;
            if (invScale.x != 0) invScale.x = 1.0f / invScale.x;
            if (invScale.y != 0) invScale.y = 1.0f / invScale.y;
            if (invScale.z != 0) invScale.z = 1.0f / invScale.z;

            Quaternion invRotation = parentRot.Inverse();

            Vector3 relativePos = invRotation.Rotate(myPos - parentPos) * invScale;
            Quaternion relativeRot = invRotation * myRot;

            relativePos = relativePos + relativeRot.Rotate(_localPosition * scale);
            relativeRot = relativeRot * _localRotation;

            if (_internal)
                _internal->SetTransform(relativePos, relativeRot);
        }
        else
        {
            Quaternion myRot = tfrm.GetRotation();
            Vector3 myPos = tfrm.GetPosition() + myRot.Rotate(_localPosition * myScale);
            myRot = myRot * _localRotation;

            if (_internal)
                _internal->SetTransform(myPos, myRot);
        }

        if (_internal)
            _internal->SetScale(myScale);
    }

    void CCollider::UpdateParentRigidbody()
    { 
        if (_isTrigger)
        {
            SetRigidBody(HRigidBody());
            return;
        }

        HSceneObject currentSO = SO();
        while (currentSO != nullptr)
        {
            HRigidBody parent = static_object_cast<CRigidBody>(currentSO->GetComponent<CRigidBody>());
            if (parent != nullptr)
            {
                if (currentSO->GetActive() && IsValidParent(parent))
                    SetRigidBody(parent);
                else
                    SetRigidBody(HRigidBody());

                return;
            }

            currentSO = currentSO->GetParent();
        }

        // Not found
        SetRigidBody(HRigidBody());
    }

    void CCollider::UpdateCollisionReportMode()
    { 
        CollisionReportMode mode = _collisionReportMode;

        if (_parent != nullptr)
            mode = _parent->GetCollisionReportMode();

        if (_internal != nullptr)
            _internal->SetCollisionReportMode(mode);
    }

    void CCollider::TriggerOnCollisionBegin(const CollisionDataRaw& data)
    { 
        CollisionData hit;
        hit.ContactPoints = data.ContactPoints;
        hit.Colliders[0] = static_object_cast<CCollider>(_thisHandle);

        if (data.Colliders[1] != nullptr)
        {
            CCollider* other = (CCollider*)data.Colliders[1]->GetOwner(PhysicsOwnerType::Component);
            hit.Colliders[1] = static_object_cast<CCollider>(other->GetHandle());
        }

        OnCollisionBegin(hit);
    }

    void CCollider::TriggerOnCollisionStay(const CollisionDataRaw& data)
    { 
        CollisionData hit;
        hit.ContactPoints = data.ContactPoints;
        hit.Colliders[0] = static_object_cast<CCollider>(_thisHandle);

        if (data.Colliders[1] != nullptr)
        {
            CCollider* other = (CCollider*)data.Colliders[1]->GetOwner(PhysicsOwnerType::Component);
            hit.Colliders[1] = static_object_cast<CCollider>(other->GetHandle());
        }

        OnCollisionStay(hit);
    }

    void CCollider::TriggerOnCollisionEnd(const CollisionDataRaw& data)
    { 
        CollisionData hit;
        hit.ContactPoints = data.ContactPoints;
        hit.Colliders[0] = static_object_cast<CCollider>(_thisHandle);

        if (data.Colliders[1] != nullptr)
        {
            CCollider* other = (CCollider*)data.Colliders[1]->GetOwner(PhysicsOwnerType::Component);
            hit.Colliders[1] = static_object_cast<CCollider>(other->GetHandle());
        }

        OnCollisionEnd(hit);
    }
}
