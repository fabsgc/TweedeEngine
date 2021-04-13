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

        /** @copydoc Body::GetPosition */
        Vector3 GetPosition() const override;

        /** @copydoc Body::GetRotation */
        Quaternion GetRotation() const override;

        /** @copydoc Body::SetTransform */
        void SetTransform(const Vector3& pos, const Quaternion& rot, bool activate = false) override;

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
        float GetFriction() const override { return _friction;  }

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

        /** @copydoc Body::AddCollider() */
        void AddCollider(Collider*) override;

        /** @copydoc Body::SyncCollider() */
        void SyncCollider(Collider*) override;

        /** @copydoc Body::RemoveCollider() */
        void RemoveCollider(Collider*) override;

        /** @copydoc Body::RemoveColliders() */
        void RemoveColliders() override;

        /** @copydoc Body::SetFlags */
        void SetFlags(BodyFlag flags) override;

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

        void Activate() const;
        bool IsActivated() const;

    private:
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

        btCompoundShape* _shape;
        Map<BulletFCollider*, ColliderData> _colliders;

        float _mass = 1.0f;
        float _friction = 0.0f;
        float _rollingFriction = 0.0f;
        float _restitution = 0.0f;
        bool _useGravity = true;
        bool _isKinematic = false;
        bool _inWorld = false;

        Vector3 _gravity = Vector3::ZERO;
        Vector3 _centerOfMass = Vector3::ZERO;
        Vector3 _position = Vector3::ZERO;
        Vector3 _velocity = Vector3::ZERO;
        Vector3 _angularVelocity = Vector3::ZERO;
        Quaternion _rotation = Quaternion::IDENTITY;

        BodyFlag _flags = (BodyFlag)((UINT32)BodyFlag::None);
    };
}