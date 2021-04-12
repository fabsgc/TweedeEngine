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
        _scale = c->_scale;
        _center = c->_center;
        _position = c->_position;
        _rotation = c->_rotation;
    }

    void CCollider::SetIsTrigger(bool value)
    {
        if (_isTrigger == value)
            return;

        _isTrigger = value;

        if (_internal != nullptr)
        {
            _internal->SetIsTrigger(value);

            UpdateParentBody();
        }
    }

    void CCollider::SetScale(const Vector3& scale)
    {
        if (_scale == scale)
            return;

        _scale = scale;

        if (_internal != nullptr)
        {
            _internal->SetScale(scale);

            if (_parent != nullptr)
                _parent->AddCollider(static_object_cast<CCollider>(_thisHandle));
        }
    }

    void CCollider::SetCenter(const Vector3& center)
    {
        if (_center == center)
            return;

        _center = center;

        if (_internal != nullptr)
        {
            _internal->SetCenter(center);

            if (_parent != nullptr)
                _parent->AddCollider(static_object_cast<CCollider>(_thisHandle));
        }
    }

    void CCollider::SetPosition(const Vector3& position)
    {
        if (_position == position)
            return;

        _position = position;

        if (_internal != nullptr)
        {
            _internal->SetPosition(position);

            if (_parent != nullptr)
                _parent->AddCollider(static_object_cast<CCollider>(_thisHandle));
        }
    }

    void CCollider::SetRotation(const Quaternion& rotation)
    {
        if (_rotation == rotation)
            return;

        _rotation = rotation;

        if (_internal != nullptr)
        {
            _internal->SetRotation(rotation);

            if (_parent != nullptr)
                _parent->AddCollider(static_object_cast<CCollider>(_thisHandle));
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
            UpdateParentBody();

        // Don't update the transform if it's due to Physics update since then we can guarantee it will remain at the same
        // relative transform to its parent
        if (gPhysics().IsUpdateInProgress())
            return;
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

        UpdateParentBody();
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

    void CCollider::SetBody(const HBody& body, bool internal)
    { 
        if (body == _parent)
            return;

        if (_internal != nullptr && !internal)
        {
            if (_parent != nullptr)
                _parent->RemoveCollider(static_object_cast<CCollider>(_thisHandle));

            Body* bodyPtr = nullptr;
            if (body != nullptr)
            {
                bodyPtr = body->GetInternal();
                _internal->SetBody(bodyPtr);

                if (bodyPtr != nullptr)
                    body->AddCollider(static_object_cast<CCollider>(_thisHandle));
            }   
        }

        _parent = body;
        UpdateCollisionReportMode();
    }

    void CCollider::UpdateParentBody()
    { 
        HSceneObject currentSO = SO();
        while (currentSO != nullptr)
        {
            HBody parent = static_object_cast<CBody>(currentSO->GetComponent<CRigidBody>());
            if (parent != nullptr)
            {
                if (currentSO->GetActive() && IsValidParent(parent))
                    SetBody(parent);
                else
                    SetBody(HBody());

                return;
            }

            currentSO = currentSO->GetParent();
        }

        // Not found
        SetBody(HBody());
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
