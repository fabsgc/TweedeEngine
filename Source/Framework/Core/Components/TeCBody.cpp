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

        _mass = c->_mass;
        _friction = c->_friction;
        _rollingFriction = c->_rollingFriction;
        _restitution = c->_restitution;
        _useGravity = c->_useGravity;
        _isKinematic = c->_isKinematic;
        _centerOfMass = c->_centerOfMass;
        _velocity = c->_velocity;
        _angularVelocity = c->_angularVelocity;
        _collisionReportMode = c->_collisionReportMode;
        _isDebug = c->_isDebug;
    }

    void CBody::Update()
    {
        if (_internal != nullptr)
            _internal->Update();
    }

    void CBody::SetIsTrigger(bool trigger)
    {
        if (_isTrigger == trigger)
            return;

        _isTrigger = trigger;

        if (_internal != nullptr)
            _internal->SetIsTrigger(trigger);
    }

    void CBody::SetIsDebug(bool debug)
    {
        if (_isDebug == debug)
            return;

        _isDebug = debug;

        if (_internal != nullptr)
            _internal->SetIsDebug(debug);
    }

    void CBody::SetMass(float mass)
    {
        if (_mass == mass)
            return;

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

    void CBody::SetVelocity(const Vector3& velocity)
    {
        if (_velocity == velocity)
            return;

        _velocity = velocity;

        if (_internal != nullptr)
            _internal->SetVelocity(velocity);
    }

    void CBody::SetAngularVelocity(const Vector3& velocity)
    {
        if (_angularVelocity == velocity)
            return;

        _angularVelocity = velocity;

        if (_internal != nullptr)
            _internal->SetAngularVelocity(velocity);
    }

    void CBody::SetFriction(float friction)
    {
        if (_friction == friction)
            return;

        _friction = friction;

        if (_internal != nullptr)
            _internal->SetFriction(friction);
    }


    void CBody::SetRollingFriction(float rollingFriction)
    {
        if (_rollingFriction == rollingFriction)
            return;

        _rollingFriction = rollingFriction;

        if (_internal != nullptr)
            _internal->SetRollingFriction(rollingFriction);
    }

    void CBody::SetRestitution(float restitution)
    {
        if (_restitution == restitution)
            return;

        _restitution = restitution;

        if (_internal != nullptr)
            _internal->SetRollingFriction(restitution);
    }

    void CBody::SetUseGravity(bool gravity)
    {
        if (_useGravity == gravity)
            return;

        _useGravity = gravity;

        if (_internal != nullptr)
            _internal->SetUseGravity(gravity);
    }

    void CBody::SetCenterOfMass(const Vector3& centerOfMass)
    {
        if (_centerOfMass == centerOfMass)
            return;

        _centerOfMass = centerOfMass;

        if (_internal != nullptr)
            _internal->SetCenterOfMass(centerOfMass);
    }

    void CBody::ApplyForce(const Vector3& force, ForceMode mode) const
    {
        if (_internal != nullptr)
            _internal->ApplyForce(force, mode);
    }

    void CBody::ApplyForceAtPoint(const Vector3& force, const Vector3& position, ForceMode mode) const
    {
        if (_internal != nullptr)
            _internal->ApplyForceAtPoint(force, position, mode);
    }

    void CBody::ApplyTorque(const Vector3& torque, ForceMode mode) const
    {
        if (_internal != nullptr)
            _internal->ApplyTorque(torque, mode);
    }

    void CBody::SetCollisionReportMode(CollisionReportMode mode)
    {
        _collisionReportMode = mode;

        if (_internal)
            _internal->SetCollisionReportMode(mode);
    }

    void CBody::SetFlags(BodyFlag flags)
    {
        _flags = flags;

        if (_internal != nullptr)
            _internal->SetFlags(flags);
    }

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
