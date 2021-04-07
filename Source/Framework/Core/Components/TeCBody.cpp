#include "Components/TeCBody.h"
#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"

namespace te
{
    CBody::CBody(UINT32 type)
        : Component(HSceneObject(), type)
        , _internal(nullptr)
    {
        SetName("Body");
        _notifyFlags = (TransformChangedFlags)(TCF_Parent | TCF_Transform);
    }

    CBody::CBody(const HSceneObject& parent, UINT32 type)
        : Component(parent, type)
        , _internal(nullptr)
    {
        SetName("Body");
        _notifyFlags = (TransformChangedFlags)(TCF_Parent | TCF_Transform);
    }

    CBody::~CBody()
    { }

    void CBody::Initialize()
    {
        Component::Initialize();
    }

    void CBody::Clone(const HComponent& c)
    {
        Clone(static_object_cast<CBody>(c));
    }

    void CBody::Clone(const HBody& c)
    {
        Component::Clone(c.GetInternalPtr());

        _collisionReportMode = c->_collisionReportMode;
    }

    void CBody::Move(const Vector3& position)
    {
        if (_internal != nullptr)
            _internal->Move(position);

        _notifyFlags = (TransformChangedFlags)0;
        SO()->SetWorldPosition(position);
        _notifyFlags = (TransformChangedFlags)(TCF_Parent | TCF_Transform);
    }

    void CBody::Rotate(const Quaternion& rotation)
    {
        if (_internal != nullptr)
            _internal->Rotate(rotation);

        _notifyFlags = (TransformChangedFlags)0;
        SO()->SetWorldRotation(rotation);
        _notifyFlags = (TransformChangedFlags)(TCF_Parent | TCF_Transform);
    }

    void CBody::SetMass(float mass)
    {
        _mass = mass;

        if (_internal != nullptr)
            _internal->SetMass(mass);
    }

    void CBody::SetIsKinematic(bool kinematic)
    {
        if (_isKinematic == kinematic)
            return;

        _isKinematic = kinematic;

        if (_internal != nullptr)
        {
            _internal->SetIsKinematic(kinematic);

            ClearColliders();
            UpdateColliders();
        }
    }

    void CBody::SetCollisionReportMode(CollisionReportMode mode)
    {
        if (_collisionReportMode == mode)
            return;

        _collisionReportMode = mode;

        for (auto& entry : _children)
            entry->UpdateCollisionReportMode();
    }

    void CBody::SetFlags(BodyFlag flags)
    {
        _flags = flags;

        if (_internal != nullptr)
        {
            _internal->SetFlags(flags);
            _internal->UpdateMassDistribution();
        }
    }

    void CBody::UpdateMassDistribution()
    {
        if (_internal != nullptr)
            return _internal->UpdateMassDistribution();
    }

    void CBody::DestroyInternal()
    { }

    void CBody::TriggerOnCollisionBegin(const CollisionDataRaw& data)
    {
        CollisionData hit;
        ProcessCollisionData(data, hit);

        OnCollisionBegin(hit);
    }

    void CBody::TriggerOnCollisionStay(const CollisionDataRaw& data)
    {
        CollisionData hit;
        ProcessCollisionData(data, hit);

        OnCollisionStay(hit);
    }

    void CBody::TriggerOnCollisionEnd(const CollisionDataRaw& data)
    {
        CollisionData hit;
        ProcessCollisionData(data, hit);

        OnCollisionEnd(hit);
    }
}
