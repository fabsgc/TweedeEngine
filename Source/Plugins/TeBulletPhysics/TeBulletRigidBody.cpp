#include "TeBulletRigidBody.h"
#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"
#include "TeBulletPhysics.h"
#include "TeBulletFBody.h"

namespace te
{
    static const float DEFAULT_MASS = 0.0f;
    static const float DEFAULT_FRICTION = 0.5f;
    static const float DEFAULT_FRICTION_ROLLING = 0.0f;
    static const float DEFAULT_RESTITUTION = 0.0f;
    static const float DEFAULT_DEACTIVATION_TIME = 2000;

    BulletRigidBody::BulletRigidBody(BulletPhysics* physics, BulletScene* scene, const HSceneObject& linkedSO)
        : RigidBody(linkedSO)
        , _rigidBody(nullptr)
        , _physics(physics)
        , _scene(scene)
    { 
        _internal = te_new<BulletFBody>(physics, scene);

        _mass = DEFAULT_MASS;
        _restitution = DEFAULT_RESTITUTION;
        _friction = DEFAULT_FRICTION;
        _frictionRolling = DEFAULT_FRICTION_ROLLING;
        _useGravity = true;
        _gravity = _physics->GetDesc().Gravity;
        _isKinematic = false;
        _inWorld = false;

        AddToWorld();
    }

    BulletRigidBody::~BulletRigidBody()
    {
        Release();
        te_delete(_internal);
    }

    void BulletRigidBody::Move(const Vector3& position)
    {
        // TODO
    }

    void BulletRigidBody::Rotate(const Quaternion& rotation)
    {
        // TODO
    }

    Vector3 BulletRigidBody::GetPosition() const
    {
        if (_rigidBody)
            return ToVector3(_rigidBody->getWorldTransform().getOrigin());

        return _position;
    }

    Quaternion BulletRigidBody::GetRotation() const
    {
        if (_rigidBody)
            return ToQuaternion(_rigidBody->getWorldTransform().getRotation());

        return _rotation;
    }

    void BulletRigidBody::SetTransform(const Vector3& pos, const Quaternion& rot, bool activate)
    {
        if (!_rigidBody)
            return;

        _position = pos;
        _rotation = rot;

        btTransform trans = _rigidBody->getWorldTransform();
        trans.setOrigin(ToBtVector3(_position));
        trans.setRotation(ToBtQuaternion(_rotation));
        _rigidBody->setWorldTransform(trans);
        _rigidBody->updateInertiaTensor();

        if (activate)
            Activate();
    }

    void BulletRigidBody::SetMass(float mass)
    {
        if (((UINT32)_flags & (UINT32)BodyFlag::AutoMass) != 0)
        {
            TE_DEBUG("Attempting to set Rigidbody mass, but it has automatic mass calculation turned on.");
            return;
        }

        mass = std::max(mass, 0.0f);
        if (mass != _mass)
        {
            _mass = mass;
            AddToWorld();
        }
    }

    float BulletRigidBody::GetMass() const
    {
        return _mass;
    }

    void BulletRigidBody::SetIsKinematic(bool kinematic)
    {
        if (kinematic == _isKinematic)
            return;

        _isKinematic = kinematic;
        AddToWorld();
    }

    bool BulletRigidBody::GetIsKinematic() const
    {
        return _isKinematic;
    }

    void BulletRigidBody::SetVelocity(const Vector3& velocity)
    {
        if (!_rigidBody)
            return;

        _velocity = velocity;
        _rigidBody->setLinearVelocity(ToBtVector3(_velocity));

        if (_velocity != Vector3::ZERO)
        {
            Activate();
        }
    }

    const Vector3& BulletRigidBody::GetVelocity() const
    {
        return _velocity;
    }

    void BulletRigidBody::SetAngularVelocity(const Vector3& velocity)
    {
        if (!_rigidBody)
            return;

        _angularVelocity = velocity;
        _rigidBody->setAngularVelocity(ToBtVector3(_angularVelocity));

        if (_angularVelocity != Vector3::ZERO)
        {
            Activate();
        }
    }

    const Vector3& BulletRigidBody::GetAngularVelocity() const
    {
        return _angularVelocity;
    }

    void BulletRigidBody::SetFriction(float friction)
    {
        if (!_rigidBody || _friction == friction)
            return;

        _friction = friction;
        _rigidBody->setFriction(friction);
    }

    float BulletRigidBody::GetFriction() const
    {
        return _friction;
    }

    void BulletRigidBody::SetRollingFriction(float rollingFriction)
    {
        if (!_rigidBody || _frictionRolling == rollingFriction)
            return;

        _frictionRolling = rollingFriction;
        _rigidBody->setRollingFriction(_frictionRolling);
    }

    float BulletRigidBody::GetRollingFriction() const
    {
        return _frictionRolling;
    }

    void BulletRigidBody::SetRestitution(float restitution)
    {
        if (!_rigidBody || _restitution == restitution)
            return;

        _restitution = restitution;
        _rigidBody->setRestitution(restitution);
    }

    float BulletRigidBody::GetRestitution() const
    {
        return _restitution;
    }

    void BulletRigidBody::SetUseGravity(bool gravity)
    {
        if (gravity == _useGravity)
            return;

        _useGravity = gravity;
        AddToWorld();
    }

    bool BulletRigidBody::GetUseGravity() const
    {
        return _useGravity;
    }

    void BulletRigidBody::SetCenterOfMass(const Vector3& centerOfMass)
    {
        if (((UINT32)_flags & (UINT32)BodyFlag::AutoTensors) != 0)
        {
            TE_DEBUG("Attempting to set Rigidbody center of mass, but it has automatic tensor calculation turned on.");
            return;
        }

        _centerOfMass = centerOfMass;
        SetTransform(GetPosition(), GetRotation());
    }

    const Vector3& BulletRigidBody::GetCenterOfMass() const
    {
        return _centerOfMass;
    }

    void BulletRigidBody::ApplyForce(const Vector3& force, ForceMode mode) const
    {
        if (!_rigidBody)
            return;

        Activate();

        if (mode == ForceMode::Force)
            _rigidBody->applyCentralForce(ToBtVector3(force));
        else if (mode == ForceMode::Impulse)
            _rigidBody->applyCentralImpulse(ToBtVector3(force));
    }

    void BulletRigidBody::ApplyForceAtPoint(const Vector3& force, const Vector3& position, ForceMode mode) const
    {
        if (!_rigidBody)
            return;

        Activate();

        if (mode == ForceMode::Force)
            _rigidBody->applyForce(ToBtVector3(force), ToBtVector3(position));
        else if (mode == ForceMode::Impulse)
            _rigidBody->applyImpulse(ToBtVector3(force), ToBtVector3(position));
    }

    void BulletRigidBody::ApplyTorque(const Vector3& torque, ForceMode mode) const
    {
        if (!_rigidBody)
            return;

        Activate();

        if (mode == ForceMode::Force)
            _rigidBody->applyTorque(ToBtVector3(torque));
        else if (mode == ForceMode::Impulse)
            _rigidBody->applyTorqueImpulse(ToBtVector3(torque));
    }

    void BulletRigidBody::AddCollider(Collider* collider)
    {
        TE_PRINT("Add Collider");
    }

    void BulletRigidBody::RemoveCollider(Collider* collider)
    {
        TE_PRINT("Remove Collider");
    }

    void BulletRigidBody::RemoveColliders()
    {
        TE_PRINT("Remove all Colliders");
    }

    void BulletRigidBody::UpdateMassDistribution()
    {
        if (((UINT32)_flags & (UINT32)BodyFlag::AutoTensors) == 0)
            return;

        if (((UINT32)_flags & (UINT32)BodyFlag::AutoMass) == 0)
        {
            // TODO
        }
        else
        {
            // TODO
        }
    }

    void BulletRigidBody::SetFlags(BodyFlag flags)
    {
        _flags = flags;
    }

    void BulletRigidBody::AddToWorld()
    {
        if (_mass < 0.0f)
            _mass = 0.0f;

        Release();

        {
            btRigidBody::btRigidBodyConstructionInfo constructionInfo(_mass, nullptr, nullptr);
            constructionInfo.m_friction = _friction;
            constructionInfo.m_rollingFriction = _frictionRolling;
            constructionInfo.m_restitution = _restitution;

            _rigidBody = te_new<btRigidBody>(constructionInfo);
            _rigidBody->setUserPointer(this);

            SetTransform(_position, _rotation);

            UpdateKinematicFlag();
            UpdateGravityFlag();

            _scene->AddRigidBody(_rigidBody);

            _inWorld = true;

            if (_mass > 0.0f)
            {
                Activate();
            }
            else
            {
                SetVelocity(Vector3::ZERO);
                SetAngularVelocity(Vector3::ZERO);
            }
        }
    }

    void BulletRigidBody::Release()
    {
        if (!_rigidBody)
            return;

        RemoveFromWorld();

        te_delete(_rigidBody);
        _rigidBody = nullptr;
    }

    void BulletRigidBody::RemoveFromWorld()
    {
        if (!_rigidBody)
            return;

        if (_inWorld)
        {
            _scene->RemoveRigidBody(_rigidBody);
            _inWorld = false;
        }
    }

    void BulletRigidBody::Activate() const
    {
        if (!_rigidBody)
            return;

        if (_mass > 0.0f)
            _rigidBody->activate(true);
    }

    bool BulletRigidBody::IsActivated() const
    {
        if(_rigidBody)
            return _rigidBody->isActive();

        return false;
    }

    void BulletRigidBody::UpdateKinematicFlag() const
    {
        int flags = _rigidBody->getCollisionFlags();

        if (_isKinematic)
            flags |= btCollisionObject::CF_KINEMATIC_OBJECT;
        else
            flags &= ~btCollisionObject::CF_KINEMATIC_OBJECT;

        _rigidBody->setCollisionFlags(flags);
        _rigidBody->forceActivationState(_isKinematic ? DISABLE_DEACTIVATION : ISLAND_SLEEPING);
        _rigidBody->setDeactivationTime(DEFAULT_DEACTIVATION_TIME);
    }

    void BulletRigidBody::UpdateGravityFlag() const
    {
        int flags = _rigidBody->getFlags();

        if (_useGravity)
            flags &= ~BT_DISABLE_WORLD_GRAVITY;
        else
            flags |= BT_DISABLE_WORLD_GRAVITY;

        _rigidBody->setFlags(flags);

        if (_useGravity)
        {
            _rigidBody->setGravity(ToBtVector3(_gravity));
        }
        else
        {
            _rigidBody->setGravity(btVector3(0.0f, 0.0f, 0.0f));
        }
    }
}
