#include "TeBulletSoftBody.h"
#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"
#include "Physics/TeCollider.h"
#include "Physics/TeJoint.h"
#include "TeBulletPhysics.h"
#include "TeBulletFCollider.h"
#include "TeBulletFBody.h"
#include "TeBulletFJoint.h"
#include "TeBulletJoint.h"

namespace te
{
    static const float DEFAULT_MASS = 1.0f;
    static const float DEFAULT_FRICTION = 0.5f;
    static const float DEFAULT_ROLLING_FRICTION = 0.0f;
    static const float DEFAULT_RESTITUTION = 0.0f;
    static const float DEFAULT_DEACTIVATION_TIME = 2000;

    BulletSoftBody::BulletSoftBody(BulletPhysics* physics, BulletScene* scene, const HSceneObject& linkedSO)
        : SoftBody(linkedSO)
        , _softBody(nullptr)
        , _physics(physics)
        , _scene(scene)
        , _isDirty(false)
    {
        _internal = te_new<BulletFBody>();

        _mass = DEFAULT_MASS;
        _restitution = DEFAULT_RESTITUTION;
        _friction = DEFAULT_FRICTION;
        _rollingFriction = DEFAULT_ROLLING_FRICTION;
        _gravity = _physics->GetDesc().Gravity;

        AddToWorld();
    }

    BulletSoftBody::~BulletSoftBody()
    { 
        te_delete((BulletFBody*)_internal);
        Release();
    }

    void BulletSoftBody::Update()
    {
        if (_isDirty && _inWorld)
            AddToWorld();

        _isDirty = false;
    }

    Vector3 BulletSoftBody::GetPosition() const
    {
        return _position;
    }

    Quaternion BulletSoftBody::GetRotation() const
    {
        return _rotation;
    }

    void BulletSoftBody::SetTransform(const Vector3& pos, const Quaternion& rot, bool activate)
    {
        // TODO
    }

    void BulletSoftBody::SetIsTrigger(bool value)
    {
        // TODO
    }

    void BulletSoftBody::SetIsDebug(bool debug)
    {
        if (_isDebug == debug)
            return;

        _isDebug = debug;
        _isDirty = true;
    }

    void BulletSoftBody::SetMass(float mass)
    {
        mass = std::max(mass, 0.0f);
        if (mass != _mass)
        {
            _mass = mass;
            _isDirty = true;
        }
    }

    void BulletSoftBody::SetIsKinematic(bool kinematic)
    {
        if (kinematic == _isKinematic)
            return;

        _isKinematic = kinematic;
        _isDirty = true;
    }

    void BulletSoftBody::SetVelocity(const Vector3& velocity)
    {
        if (!_softBody)
            return;

        _velocity = velocity;
        _softBody->setLinearVelocity(ToBtVector3(_velocity));

        if (_velocity != Vector3::ZERO)
            Activate();
    }

    void BulletSoftBody::SetAngularVelocity(const Vector3& velocity)
    {
        if (!_softBody)
            return;

        _angularVelocity = velocity;
        _softBody->setAngularVelocity(ToBtVector3(_angularVelocity));

        if (_angularVelocity != Vector3::ZERO)
            Activate();
    }

    void BulletSoftBody::SetFriction(float friction)
    {
        if (!_softBody)
            return;

        _friction = friction;
        _softBody->setFriction(friction);
    }

    void BulletSoftBody::SetRollingFriction(float rollingFriction)
    {
        if (!_softBody)
            return;

        _rollingFriction = rollingFriction;
        _softBody->setRollingFriction(_rollingFriction);
    }

    void BulletSoftBody::SetRestitution(float restitution)
    {
        if (!_softBody)
            return;

        _restitution = restitution;
        _softBody->setRestitution(restitution);
    }

    void BulletSoftBody::SetUseGravity(bool gravity)
    {
        // TODO
    }

    void BulletSoftBody::SetFlags(BodyFlag flags)
    {
        if (_flags == flags)
            return;

        _flags = flags;
        UpdateCCDFlag();
    }

    void BulletSoftBody::SetCenterOfMass(const Vector3& centerOfMass)
    {
        // TODO
    }

    void BulletSoftBody::ApplyForce(const Vector3& force, ForceMode mode) const
    {
        // TODO
    }

    void BulletSoftBody::ApplyForceAtPoint(const Vector3& force, const Vector3& position, ForceMode mode) const
    {
        // TODO
    }

    void BulletSoftBody::ApplyTorque(const Vector3& torque, ForceMode mode) const
    {
        // TODO
    }

    void BulletSoftBody::SetCollisionReportMode(CollisionReportMode mode)
    {
        _collisionReportMode = mode;
    }

    void BulletSoftBody::AddCollider(Collider* collider)
    {
        // TODO
    }

    void BulletSoftBody::RemoveCollider(Collider* collider)
    {
        // TODO
    }

    void BulletSoftBody::RemoveColliders()
    {
        // TODO
    }

    void BulletSoftBody::AddJoint(Joint* joint)
    {
        // TODO
    }

    void BulletSoftBody::RemoveJoint(Joint* joint)
    {
        // TODO
    }

    void BulletSoftBody::RemoveJoints()
    {
        // TODO
    }

    void BulletSoftBody::AddToWorld()
    {
        // TODO
    }

    void BulletSoftBody::Release()
    {
        // TODO
    }

    void BulletSoftBody::RemoveFromWorld()
    {
        // TODO
    }

    void BulletSoftBody::Activate() const
    {
        if (!_softBody)
            return;

        if (_mass > 0.0f)
            _softBody->activate(true);
    }

    bool BulletSoftBody::IsActivated() const
    {
        if (_softBody)
            return _softBody->isActive();

        return false;
    }

    void BulletSoftBody::UpdateKinematicFlag() const
    {
        int flags = _softBody->getCollisionFlags();

        if (_isKinematic)
            flags |= btCollisionObject::CF_KINEMATIC_OBJECT;
        else
            flags &= ~btCollisionObject::CF_KINEMATIC_OBJECT;

        if (_isTrigger)
            flags |= btCollisionObject::CF_NO_CONTACT_RESPONSE;
        else
            flags &= ~btCollisionObject::CF_NO_CONTACT_RESPONSE;

        if (_isDebug)
            flags &= ~btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT;
        else
            flags |= btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT;

        _softBody->setCollisionFlags(flags);
        _softBody->forceActivationState(_isKinematic ? DISABLE_DEACTIVATION : ISLAND_SLEEPING);
        _softBody->setDeactivationTime(DEFAULT_DEACTIVATION_TIME);
    }

    void BulletSoftBody::UpdateGravityFlag() const
    {
        // TODO
    }

    void BulletSoftBody::UpdateCCDFlag() const
    {
        if (((UINT32)_flags & (UINT32)BodyFlag::CCD))
        {
            _softBody->setCcdMotionThreshold(0.015f);
            _softBody->setCcdSweptSphereRadius(0.01f);
        }
        else
        {
            _softBody->setCcdMotionThreshold(std::numeric_limits<float>::infinity());
            _softBody->setCcdSweptSphereRadius(0);
        }
    }
}
