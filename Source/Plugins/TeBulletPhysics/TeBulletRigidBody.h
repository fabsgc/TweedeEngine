#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeRigidBody.h"
#include "Math/TeVector3.h"
#include "Math/TeQuaternion.h"

namespace te
{
    class BulletFCollider;
    class MotionState;

    /** Bullet implementation of a RigidBody. */
    class BulletRigidBody : public RigidBody
    {
    public:
        BulletRigidBody(BulletPhysics* physics, BulletScene* scene, const HSceneObject& linkedSO);
        ~BulletRigidBody();

        /** @copydoc Body::Update */
        void Update() override;

        /** @copydoc Body::GetPosition */
        Vector3 GetPosition() const override;

        /** @copydoc Body::GetRotation */
        Quaternion GetRotation() const override;

        /** @copydoc Body::SetTransform */
        void SetTransform(const Vector3& pos, const Quaternion& rot, bool activate = false) override;

        /** @copydoc Body::SetIsTrigger */
        void SetIsTrigger(bool trigger) override;

        /** @copydoc Body::GetIsTrigger */
        bool GetIsTrigger() const  override { return _isTrigger; }

        /** @copydoc Body::SetIsTrigger */
        void SetIsDebug(bool debug) override;

        /** @copydoc Body::GetIsTrigger */
        bool GetIsDebug() const override { return _isDebug; }

        /** @copydoc Body::SetMass */
        void SetMass(float mass) override;

        /** @copydoc Body::GetMass */
        float GetMass() const override { return _mass; }

        /** @copydoc Body::SetIsKinematic */
        void SetIsKinematic(bool kinematic) override;

        /** @copydoc Body::GetIsKinematic */
        bool GetIsKinematic() const override { return _isKinematic; }

        /** @copydoc Body::SetVelocity */
        void SetVelocity(const Vector3& velocity) override;

        /** @copydoc Body::GetVelocity */
        const Vector3& GetVelocity() const override { return _velocity; }

        /** @copydoc Body::SetAngularVelocity */
        void SetAngularVelocity(const Vector3& velocity) override;

        /** @copydoc Body::GetAngularVelocity */
        const Vector3& GetAngularVelocity() const override { return _angularVelocity; }

        /** @copydoc Body::SetFriction */
        void SetFriction(float friction) override;

        /** @copydoc Body::GetFriction */
        float GetFriction() const override { return _friction; }

        /** @copydoc Body::SetRollingFriction */
        void SetRollingFriction(float rollingFriction) override;

        /** @copydoc Body::GetRollingFriction */
        float GetRollingFriction() const override { return _rollingFriction; }

        /** @copydoc Body::SetRestitution */
        void SetRestitution(float restitution) override;

        /** @copydoc Body::GetRestitution */
        float GetRestitution() const override { return _restitution; }

        /** @copydoc Body::SetUseGravity */
        void SetUseGravity(bool gravity) override;

        /** @copydoc Body::GetUseGravity */
        bool GetUseGravity() const override { return _useGravity; }

        /** @copydoc Body::SetCenterOfMass */
        void SetCenterOfMass(const Vector3& centerOfMass) override;

        /** @copydoc Body::GetCenterOfMass */
        const Vector3& GetCenterOfMass() const override;

        /** @copydoc Body::ApplyForce */
        void ApplyForce(const Vector3& force, ForceMode mode) const override;

        /** @copydoc Body::ApplyForceAtPoint */
        void ApplyForceAtPoint(const Vector3& force, const Vector3& position, ForceMode mode) const override;

        /** @copydoc Body::ApplyTorque */
        void ApplyTorque(const Vector3& torque, ForceMode mode) const override;

        /** @copydoc Body::SetCollisionReportMode */
        void SetCollisionReportMode(CollisionReportMode mode) override;

        /** @copydoc Body::GetCollisionReportMode */
        CollisionReportMode GetCollisionReportMode() const { return _collisionReportMode; }

        /** @copydoc Body::AddCollider */
        void AddCollider(Collider*) override;

        /** @copydoc Body::RemoveCollider */
        void RemoveCollider(Collider*) override;

        /** @copydoc Body::RemoveColliders */
        void RemoveColliders() override;

        /** @copydoc Body::AddJoint */
        void AddJoint(Joint* joint) override;

        /** @copydoc Body::RemoveJoint */
        void RemoveJoint(Joint* joint) override;

        /** @copydoc Body::RemoveJoint */
        void RemoveJoint(BulletJoint* joint);

        /** @copydoc Body::RemoveJoints */
        void RemoveJoints() override;

        /** @copydoc Body::SetFlags */
        void SetFlags(BodyFlag flags) override;

        /** @copydoc RigidBody::SetAngularFactor */
        void SetAngularFactor(const Vector3& angularFactor) override;

        /** @copydoc RigidBody::GetAngularFactor */
        const Vector3& GetAngularFactor() const override;

    private:
        /** Add RigidBody to world */
        void AddToWorld();

        /** Release Body from simulation */
        void Release();

        /** Remove RigidBody from world */
        void RemoveFromWorld();

        /** Update kinematic bullet flag */
        void UpdateKinematicFlag() const;

        /** Update gravity bullet flag */
        void UpdateGravityFlag() const;

        /** Enable or disable CCD for this body */
        void UpdateCCDFlag() const;

        /** Activate btRigidBody */
        void Activate() const;

        /** Check if btRigidBody is activated */
        bool IsActivated() const;

    private:
        /** Currently this struct is not usefull */
        struct ColliderData
        {
            UINT32 Index = 0;

            ColliderData(UINT32 index = 0)
                : Index(index)
            { }
        };

    private:
        friend class MotionState;

        btRigidBody* _rigidBody;
        BulletPhysics* _physics;
        BulletScene* _scene;

        bool _isDirty = true; // A state has been modified

        btCompoundShape* _shape;
        UnorderedMap<BulletFCollider*, ColliderData> _colliders;
        Vector<BulletJoint*> _joints;

        float _mass = 1.0f;
        float _friction = 0.0f;
        float _rollingFriction = 0.0f;
        float _restitution = 0.0f;
        bool _useGravity = true;
        bool _isKinematic = false;
        bool _inWorld = false;
        bool _isTrigger = false;
        bool _isDebug = true;

        Vector3 _gravity = Vector3::ZERO;
        Vector3 _centerOfMass = Vector3::ZERO;
        Vector3 _position = Vector3::ZERO;
        Vector3 _velocity = Vector3::ZERO;
        Vector3 _angularVelocity = Vector3::ZERO;
        Vector3 _angularFactor = Vector3::ONE;
        Quaternion _rotation = Quaternion::IDENTITY;

        BodyFlag _flags = (BodyFlag)((UINT32)BodyFlag::None);
    };
}
