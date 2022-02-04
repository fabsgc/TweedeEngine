#include "TeBulletRigidBody.h"
#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"
#include "Physics/TeCollider.h"
#include "Physics/TeJoint.h"
#include "Math/TeAABox.h"
#include "TeBulletPhysics.h"
#include "TeBulletFCollider.h"
#include "TeBulletFBody.h"
#include "TeBulletFJoint.h"
#include "TeBulletJoint.h"

namespace te
{
    static const float DEFAULT_MASS = 1.0f;
    static const float DEFAULT_FRICTION = 0.5f;
    static const float DEFAULT_ROLLING_FRICTION = 0.2f;
    static const float DEFAULT_RESTITUTION = 0.1f;
    static const float DEFAULT_DEACTIVATION_TIME = 2000;

    class MotionState : public btMotionState
    {
    public:
        MotionState(BulletRigidBody* rigidBody) { _rigidBody = rigidBody; }

        // Update from engine, ENGINE -> BULLET
        void getWorldTransform(btTransform& worldTrans) const override
        {
            const Vector3 lastPos = _rigidBody->GetPosition();
            const Quaternion lastRot = _rigidBody->GetRotation();

            worldTrans.setOrigin(ToBtVector3(lastPos + lastRot * _rigidBody->GetCenterOfMass()));
            worldTrans.setRotation(ToBtQuaternion(lastRot));
        }

        // Update from bullet, BULLET -> ENGINE
        void setWorldTransform(const btTransform& worldTrans) override
        {
            const Quaternion newWorldRot = ToQuaternion(worldTrans.getRotation());
            const Vector3 newWorldPos = ToVector3(worldTrans.getOrigin()) - newWorldRot * _rigidBody->GetCenterOfMass();

            _rigidBody->_setTransform(newWorldPos, newWorldRot);

            _rigidBody->_position = newWorldPos;
            _rigidBody->_rotation = newWorldRot;
        }
    private:
        BulletRigidBody* _rigidBody;
    };

    BulletRigidBody::BulletRigidBody(BulletPhysics* physics, BulletScene* scene, const HSceneObject& linkedSO)
        : RigidBody(linkedSO)
        , _rigidBody(nullptr)
        , _physics(physics)
        , _scene(scene)
        , _isDirty(false)
        , _shape(nullptr)
    {
        _mass = DEFAULT_MASS;
        _friction = DEFAULT_FRICTION;
        _restitution = DEFAULT_RESTITUTION;
        _rollingFriction = DEFAULT_ROLLING_FRICTION;
        _gravity = _physics->GetDesc().Gravity;

        _internal = te_new<BulletFBody>();

        AddToWorld();
    }

    BulletRigidBody::~BulletRigidBody()
    {
        _colliders.clear();
        _joints.clear();

        te_delete((BulletFBody*)_internal);
        Release();
    }

    void BulletRigidBody::Update()
    {
        if (_isDirty && _inWorld)
            AddToWorld();

        _isDirty = false;
    }

    Vector3 BulletRigidBody::GetPosition() const
    {
        if (_rigidBody)
        {
            if (((UINT32)_flags & (UINT32)BodyFlag::CCD) != 0)
                return ToVector3(_rigidBody->getInterpolationWorldTransform().getOrigin());
            else
                return ToVector3(_rigidBody->getWorldTransform().getOrigin());
        }

        return _position;
    }

    Quaternion BulletRigidBody::GetRotation() const
    {
        if (_rigidBody)
        {
            if (((UINT32)_flags & (UINT32)BodyFlag::CCD) != 0)
                return ToQuaternion(_rigidBody->getInterpolationWorldTransform().getRotation());
            else
                return ToQuaternion(_rigidBody->getWorldTransform().getRotation());
        }

        return _rotation;
    }

    void BulletRigidBody::SetTransform(const Vector3& position, const Quaternion& rotation)
    {
        _position = position;
        _rotation = rotation;

        if (_rigidBody)
        {
            // Set position and rotation to world transform
            const Vector3 oldPosition = GetPosition();
            btTransform& trans = _rigidBody->getWorldTransform();
            trans.setOrigin(ToBtVector3(_position + ToQuaternion(trans.getRotation()) * _centerOfMass));
            trans.setRotation(ToBtQuaternion(_rotation));

            if (_centerOfMass != Vector3::ZERO)
                trans.setOrigin(ToBtVector3(oldPosition + _rotation * _centerOfMass));

            // Set position and rotation to interpolated world transform
            btTransform transInterpolated = _rigidBody->getInterpolationWorldTransform();
            transInterpolated.setRotation(trans.getRotation());
            transInterpolated.setOrigin(trans.getOrigin());

            if (_centerOfMass != Vector3::ZERO)
                transInterpolated.setOrigin(trans.getOrigin());

            _rigidBody->setInterpolationWorldTransform(transInterpolated);
            _rigidBody->updateInertiaTensor();
        }
    }

    AABox BulletRigidBody::GetBoundingBox() const
    {
        return AABox();
    }

    void BulletRigidBody::SetIsTrigger(bool trigger)
    {
        if (_isTrigger == trigger)
            return;

        _isTrigger = trigger;
        UpdateKinematicFlag();
    }

    void BulletRigidBody::SetIsDebug(bool debug)
    {
        if (_isDebug == debug)
            return;

        _isDebug = debug;
        UpdateKinematicFlag();
    }

    void BulletRigidBody::SetMass(float mass)
    {
        mass = std::max(mass, 0.0f);
        if (mass != _mass)
        {
            _mass = mass;
            _isDirty = true;
        }
    }

    void BulletRigidBody::SetIsKinematic(bool kinematic)
    {
        if (kinematic == _isKinematic)
            return;

        _isKinematic = kinematic;
        UpdateKinematicFlag();
    }

    void BulletRigidBody::SetVelocity(const Vector3& velocity)
    {
        if (_velocity == velocity)
            return;

        _velocity = velocity;

        if (_rigidBody)
        {
            _rigidBody->setLinearVelocity(ToBtVector3(_velocity));

            if (_velocity != Vector3::ZERO)
                Activate();
        }
    }

    void BulletRigidBody::SetAngularVelocity(const Vector3& velocity)
    {
        if (_angularVelocity == velocity)
            return;

        _angularVelocity = velocity;

        if (_rigidBody)
        {
            _rigidBody->setAngularVelocity(ToBtVector3(_angularVelocity));

            if (_angularVelocity != Vector3::ZERO)
                Activate();
        }
    }

    void BulletRigidBody::SetFriction(float friction)
    {
        if (_friction == friction)
            return;

        _friction = friction;

        if (_rigidBody)
            _rigidBody->setFriction(friction);
    }

    void BulletRigidBody::SetRollingFriction(float rollingFriction)
    {
        if (_rollingFriction == rollingFriction)
            return;

        _rollingFriction = rollingFriction;

        if (_rigidBody)
            _rigidBody->setRollingFriction(rollingFriction);
    }

    void BulletRigidBody::SetRestitution(float restitution)
    {
        if (_restitution == restitution)
            return;

        _restitution = restitution;

        if (_rigidBody)
            _rigidBody->setRestitution(restitution);
    }

    void BulletRigidBody::SetUseGravity(bool gravity)
    {
        if (gravity == _useGravity)
            return;

        _useGravity = gravity;
        UpdateGravityFlag();
    }

    void BulletRigidBody::SetFlags(BodyFlag flags)
    {
        if (_flags == flags)
            return;

        _flags = flags;
        UpdateCCDFlag();
    }

    void BulletRigidBody::SetCenterOfMass(const Vector3& centerOfMass)
    {
        if (_centerOfMass == centerOfMass)
            return;

        _centerOfMass = centerOfMass;
        SetTransform(GetPosition(), GetRotation());
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

    void BulletRigidBody::SetCollisionReportMode(CollisionReportMode mode)
    {
        _collisionReportMode = mode;
    }

    void BulletRigidBody::SetAngularFactor(const Vector3& angularFactor)
    {
        if (_angularFactor == angularFactor)
            return;

        _angularFactor = angularFactor;

        if (!_rigidBody)
            _rigidBody->setAngularFactor(ToBtVector3(_angularFactor));        
    }

    const Vector3& BulletRigidBody::GetAngularFactor() const
    {
        return _angularFactor;
    }

    void BulletRigidBody::AddCollider(Collider* collider)
    {
        BulletFCollider* fCollider = static_cast<BulletFCollider*>(collider->GetInternal());
        auto it = _colliders.find(fCollider);
        if (it == _colliders.end())
        {
            UINT32 index = (UINT32)_colliders.size();
            _colliders[fCollider] = ColliderData(index);
        }

        AddToWorld();
    }

    void BulletRigidBody::RemoveCollider(Collider* collider)
    {
        BulletFCollider* fCollider = static_cast<BulletFCollider*>(collider->GetInternal());
        auto it = _colliders.find(fCollider);
        if (it != _colliders.end())
        {
            if(fCollider->GetShape())
                _shape->removeChildShape(fCollider->GetShape());
            
            _shape->recalculateLocalAabb();
            _colliders.erase(it);
        }

        if (_rigidBody)
            _rigidBody->updateInertiaTensor();

        if(_colliders.size() == 0 && _inWorld)
            RemoveFromWorld();
        else if(_colliders.size() > 0)
            AddToWorld();
    }

    void BulletRigidBody::RemoveColliders()
    {
        for (auto& collider : _colliders)
        {
            if(collider.first->GetShape())
                _shape->removeChildShape(collider.first->GetShape());
        }

        _colliders.clear();

        if (_inWorld)
            RemoveFromWorld();
    }

    void BulletRigidBody::AddJoint(Joint* joint)
    {
        // Depending on the order of components activations, sometimes we call AddJoint with a nullptr joint
        // This is not an issue, it means that we try to create the rigid body before its attached joints
        if (!joint)
            return;

        BulletFJoint* fJoint = ((BulletFJoint*)joint->GetInternal());
        BulletJoint* bJoint = fJoint->GetJoint();

        auto it = std::find(_joints.begin(), _joints.end(), bJoint);
        if (it == _joints.end())
            _joints.push_back(bJoint);

        AddToWorld();
    }

    void BulletRigidBody::RemoveJoint(Joint* joint)
    {
        if (_joints.size() == 0)
            return;

        BulletFJoint* fJoint = ((BulletFJoint*)joint->GetInternal());

        // Depending on the order of components activations, sometimes we call AddJoint with a nullptr joint
        // This is not an issue, it means that we try to create the rigid body before its attached joints
        if (!fJoint)
            return;

        BulletJoint* bJoint = fJoint->GetJoint();

        auto it = std::find(_joints.begin(), _joints.end(), bJoint);
        if (it != _joints.end())
        {
            bJoint->ReleaseJoint();
            _joints.erase(it);
        }

        AddToWorld();

        bJoint->BuildJoint();
    }

    void BulletRigidBody::RemoveJoint(BulletJoint* joint)
    {
        auto it = std::find(_joints.begin(), _joints.end(), joint);
        if (it != _joints.end())
            _joints.erase(it);

        AddToWorld();
    }

    void BulletRigidBody::RemoveJoints()
    {
        _joints.clear();
        AddToWorld();
    }

    void BulletRigidBody::AddToWorld()
    {
        if (_mass < 0.0f)
            _mass = 0.0f;

        btVector3 localInertia = btVector3(0, 0, 0);
        Release();

        // Add child shapes
        _shape = te_new<btCompoundShape>();
        for (auto& collider : _colliders)
        {
            if(collider.first->GetShape())
                _shape->addChildShape(collider.first->GetBtTransform(), collider.first->GetShape());
        }

        _shape->calculateLocalInertia(_mass, localInertia);

        // Create a motion state (memory will be freed by the RigidBody)
        const auto motionState = te_new<MotionState>(this);

        btRigidBody::btRigidBodyConstructionInfo constructionInfo(_mass, motionState, _shape, localInertia);
        constructionInfo.m_friction = _friction;
        constructionInfo.m_rollingFriction = _rollingFriction;
        constructionInfo.m_restitution = _restitution;
        constructionInfo.m_localInertia = localInertia;
        constructionInfo.m_collisionShape = _shape;
        constructionInfo.m_motionState = motionState;

        _rigidBody = te_new<btRigidBody>(constructionInfo);
        _rigidBody->setUserPointer(this);

        ((BulletFBody*)_internal)->SetBody(_rigidBody);

        if (_colliders.size() > 0)
        {
            _rigidBody->setFriction((btScalar)_friction);
            _rigidBody->setRestitution((btScalar)_restitution);
            _rigidBody->setAngularFactor(ToBtVector3(_angularFactor));
            _rigidBody->setRollingFriction((btScalar)_rollingFriction);

            if (_mass > 0.0f)
            {
                Activate();
                _rigidBody->setLinearVelocity(ToBtVector3(_velocity));
                _rigidBody->setAngularVelocity(ToBtVector3(_angularVelocity));
            }
            else
            {
                _rigidBody->setLinearVelocity(ToBtVector3(Vector3::ZERO));
                _rigidBody->setAngularVelocity(ToBtVector3(Vector3::ZERO));
            }

            UpdateKinematicFlag();
            UpdateGravityFlag();
            UpdateCCDFlag();

            SetTransform(_position, _rotation);

            _scene->AddRigidBody(_rigidBody);
            _inWorld = true;
            _isDirty = false;
        }

        for (auto joint : _joints)
            joint->BuildJoint();
    }

    void BulletRigidBody::Release()
    {
        if (!_rigidBody)
            return;

        RemoveFromWorld();

        for (auto joint : _joints)
            joint->ReleaseJoint();

        te_delete(_rigidBody->getMotionState());
        te_delete(_rigidBody);

        te_delete(_shape);

        ((BulletFBody*)_internal)->SetBody(nullptr);

        _rigidBody = nullptr;
        _shape = nullptr;
    }

    void BulletRigidBody::RemoveFromWorld()
    {
        if (!_rigidBody)
            return;

        if (_inWorld)
        {
            _rigidBody->activate(false);
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
        if (!_rigidBody)
            return;

        int flags = _rigidBody->getCollisionFlags();

        if (_isKinematic)
            flags |= btCollisionObject::CF_KINEMATIC_OBJECT;
        else
            flags &= ~btCollisionObject::CF_KINEMATIC_OBJECT;

        if(_isTrigger)
            flags |= btCollisionObject::CF_NO_CONTACT_RESPONSE;
        else
            flags &= ~btCollisionObject::CF_NO_CONTACT_RESPONSE;

        if(_isDebug)
            flags &= ~btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT;
        else
            flags |= btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT;

        _rigidBody->setCollisionFlags(flags);
        _rigidBody->forceActivationState(DISABLE_DEACTIVATION);
        _rigidBody->setDeactivationTime(DEFAULT_DEACTIVATION_TIME);
    }

    void BulletRigidBody::UpdateGravityFlag() const
    {
        if (!_rigidBody)
            return;

        int flags = _rigidBody->getFlags();

        if (_useGravity)
            flags &= ~BT_DISABLE_WORLD_GRAVITY;
        else
            flags |= BT_DISABLE_WORLD_GRAVITY;

        _rigidBody->setFlags(flags);

        if (_useGravity)
            _rigidBody->setGravity(ToBtVector3(_gravity));
        else
            _rigidBody->setGravity(btVector3(0.0f, 0.0f, 0.0f));
    }

    void BulletRigidBody::UpdateCCDFlag() const
    {
        if (!_rigidBody)
            return;

        if (((UINT32)_flags & (UINT32)BodyFlag::CCD))
        {
            _rigidBody->setCcdMotionThreshold(0.015f);
            _rigidBody->setCcdSweptSphereRadius(0.01f);
        }
        else
        {
            _rigidBody->setCcdMotionThreshold(std::numeric_limits<float>::infinity());
            _rigidBody->setCcdSweptSphereRadius(0);
        }
    }
}
