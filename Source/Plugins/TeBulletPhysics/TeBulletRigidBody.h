#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeRigidBody.h"
#include "Math/TeVector3.h"
#include "Math/TeQuaternion.h"

namespace te
{
    /** Bullet implementation of a RigidBody. */
    class BulletRigidBody : public RigidBody
    {
    public:
        BulletRigidBody(BulletPhysics* physics, BulletScene* scene, const HSceneObject& linkedSO);
        ~BulletRigidBody();

        /** @copydoc Body::Move */
        void Move(const Vector3& position) override;

        /** @copydoc Body::Rotate */
        void Rotate(const Quaternion& rotation) override;

        /** @copydoc Body::GetPosition */
        Vector3 GetPosition() const override;

        /** @copydoc Body::GetRotation */
        Quaternion GetRotation() const override;

        /** @copydoc Body::SetTransform */
        void SetTransform(const Vector3& pos, const Quaternion& rot) override;

        /** @copydoc Body::SetMass */
        void SetMass(float mass) override;

        /** @copydoc Body::GetMass */
        float GetMass() const override;

        /** @copydoc Body::SetIsKinematic */
        void SetIsKinematic(bool kinematic);

        /** @copydoc Body::GetIsKinematic */
        bool GetIsKinematic() const;

        /** @copydoc Body::SetVelocity */
        void SetVelocity(const Vector3& velocity);

        /** @copydoc Body::GetVelocity */
        const Vector3& GetVelocity() const;

        /** @copydoc Body::SetAngularVelocity */
        void SetAngularVelocity(const Vector3& velocity);

        /** @copydoc Body::GetAngularVelocity */
        const Vector3& GetAngularVelocity() const;

        /** @copydoc Body::SetFriction */
        void SetFriction(float friction);

        /** @copydoc Body::GetFriction */
        float GetFriction() const;

        /** @copydoc Body::SetRollingFriction */
        void SetRollingFriction(float rollingFriction);

        /** @copydoc Body::GetRollingFriction */
        float GetRollingFriction() const;

        /** @copydoc Body::SetRestitution */
        void SetRestitution(float restitution);

        /** @copydoc Body::GetRestitution */
        float GetRestitution() const;

        /** @copydoc Body::SetUseGravity */
        void SetUseGravity(bool gravity);

        /** @copydoc Body::GetUseGravity */
        bool GetUseGravity() const;

        /** @copydoc Body::SetCenterOfMass */
        void SetCenterOfMass(const Vector3& centerOfMass);

        /** @copydoc Body::GetCenterOfMass */
        const Vector3& GetCenterOfMass() const;

        /** @copydoc Body::ApplyForce */
        void ApplyForce(const Vector3& force, ForceMode mode) const;

        /** @copydoc Body::ApplyForceAtPoint */
        void ApplyForceAtPoint(const Vector3& force, const Vector3& position, ForceMode mode) const;

        /** @copydoc Body::ApplyTorque */
        void ApplyTorque(const Vector3& torque, ForceMode mode) const;

        /** @copydoc Body::AddCollider() */
        void AddCollider(Collider* collider) override;

        /** @copydoc Body::RemoveCollider() */
        void RemoveCollider(Collider* collider) override;

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

        void Activate() const;
        bool IsActivated() const;

    private:
        btRigidBody* _rigidBody;
        BulletPhysics* _physics;
        BulletScene* _scene;

        float _mass = 0.0f;
        float _friction = 0.0f;
        float _frictionRolling = 0.0f;
        float _restitution = 0.0f;
        bool _useGravity = false;
        bool _isKinematic = false;
        bool _inWorld = false;
        Vector3 _gravity = Vector3::ZERO;
        Vector3 _centerOfMass = Vector3::ZERO;
        Vector3 _position = Vector3::ZERO;
        Vector3 _velocity = Vector3::ZERO;
        Vector3 _angularVelocity = Vector3::ZERO;
        Quaternion _rotation = Quaternion::IDENTITY;
        BodyFlag _flags = (BodyFlag)((UINT32)BodyFlag::AutoTensors | (UINT32)BodyFlag::AutoMass);
    };
}
