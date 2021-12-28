#include "TeBulletFSoftBody.h"
#include "TeBulletSoftBody.h"
#include "Math/TeAABox.h"

namespace te
{ 
    static const float DEFAULT_MASS = 1.0f;
    static const float DEFAULT_FRICTION = 0.5f;
    static const float DEFAULT_ROLLING_FRICTION = 0.2f;
    static const float DEFAULT_RESTITUTION = 0.1f;

    BulletFSoftBody::BulletFSoftBody(BulletSoftBody* body, btSoftBody* btBody)
        : FSoftBody()
        , _softBody(body)
        , _btSoftBody(btBody)
    { 
        _mass = DEFAULT_MASS;
        _friction = DEFAULT_FRICTION;
        _restitution = DEFAULT_RESTITUTION;
        _rollingFriction = DEFAULT_ROLLING_FRICTION;
    }

    void BulletFSoftBody::SetScale(const Vector3& scale)
    {
        if (_scale == scale)
            return;

        _scale = scale;
        _softBody->AddToWorld();
    }

    const Vector3& BulletFSoftBody::GetScale() const
    {
        return _scale;
    }

    Vector3 BulletFSoftBody::GetPosition() const
    {
        if (_btSoftBody)
        {
            if (((UINT32)_flags & (UINT32)BodyFlag::CCD) != 0)
                return ToVector3(_btSoftBody->getInterpolationWorldTransform().getOrigin());
            else
                return ToVector3(_btSoftBody->getWorldTransform().getOrigin());
        }

        return _position;
    }

    Quaternion BulletFSoftBody::GetRotation() const
    {
        if (_btSoftBody)
        {
            if (((UINT32)_flags & (UINT32)BodyFlag::CCD) != 0)
                return ToQuaternion(_btSoftBody->getInterpolationWorldTransform().getRotation());
            else
                return ToQuaternion(_btSoftBody->getWorldTransform().getRotation());
        }

        return _rotation;
    }

    AABox BulletFSoftBody::GetBoundingBox() const
    {
        btVector3 min, max;
        _btSoftBody->getAabb(min, max);

        return AABox(ToVector3(min), ToVector3(max));
    }

    void BulletFSoftBody::SetTransform(const Vector3& position, const Quaternion& rotation)
    {
        if (_position == position && _rotation == rotation)
            return;

        _position = position;
        _rotation = rotation;

        if (_btSoftBody)
            _btSoftBody->transformTo(btTransform(ToBtQuaternion(_rotation), ToBtVector3(_position)));
    }

    void BulletFSoftBody::SetIsTrigger(bool trigger)
    {
        if (_isTrigger == trigger)
            return;

        _isTrigger = trigger;
        _softBody->UpdateKinematicFlag(this);
    }

    void BulletFSoftBody::SetIsDebug(bool debug)
    {
        if (_isDebug == debug)
            return;

        _isDebug = debug;
        _softBody->UpdateKinematicFlag(this);
    }

    void BulletFSoftBody::SetMass(float mass)
    {
        mass = std::max(mass, 0.0f);
        if (mass == _mass)
            return;

        _mass = mass;

        if (_btSoftBody)
            _btSoftBody->setTotalMass(_mass);
    }

    void BulletFSoftBody::SetIsKinematic(bool kinematic)
    {
        if (kinematic == _isKinematic)
            return;

        _isKinematic = kinematic;
        _softBody->UpdateKinematicFlag(this);
    }

    void BulletFSoftBody::SetVelocity(const Vector3& velocity)
    {
        if (_velocity == velocity)
            return;

        _velocity = velocity;

        if (_btSoftBody)
        {
            _btSoftBody->setLinearVelocity(ToBtVector3(_velocity));

            if (_velocity != Vector3::ZERO)
                _softBody->Activate(this);
        }
    }

    void BulletFSoftBody::SetAngularVelocity(const Vector3& velocity)
    {
        if (_angularVelocity == velocity)
            return;

        _angularVelocity = velocity;

        if (_btSoftBody)
        {
            _btSoftBody->setAngularVelocity(ToBtVector3(_angularVelocity));

            if (_angularVelocity != Vector3::ZERO)
                _softBody->Activate(this);
        }
    }

    void BulletFSoftBody::SetFriction(float friction)
    {
        if (_friction == friction)
            return;

        _friction = friction;

        if (_btSoftBody)
            _btSoftBody->setFriction(friction);
    }

    void BulletFSoftBody::SetRollingFriction(float rollingFriction)
    {
        if (_rollingFriction == rollingFriction)
            return;

        _rollingFriction = rollingFriction;

        if (_btSoftBody)
            _btSoftBody->setRollingFriction(rollingFriction);
    }

    void BulletFSoftBody::SetRestitution(float restitution)
    {
        if (_restitution == restitution)
            return;

        _restitution = restitution;

        if (_btSoftBody)
            _btSoftBody->setRestitution(restitution);
    }

    void BulletFSoftBody::SetFlags(BodyFlag flags)
    {
        if (_flags == flags)
            return;

        _flags = flags;
        _softBody->UpdateCCDFlag(this);
    }

    void BulletFSoftBody::ApplyForce(const Vector3& force, ForceMode mode) const
    {
        // TODO
    }

    void BulletFSoftBody::ApplyForceAtPoint(const Vector3& force, const Vector3& position, ForceMode mode) const
    {
        // TODO
    }

    void BulletFSoftBody::ApplyTorque(const Vector3& torque, ForceMode mode) const
    {
        // TODO
    }

    void BulletFSoftBody::SetCollisionReportMode(CollisionReportMode mode)
    {
        _collisionReportMode = mode;
    }
}
