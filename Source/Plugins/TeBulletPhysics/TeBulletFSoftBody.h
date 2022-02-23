#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeFSoftBody.h"
#include "Physics/TeBody.h"

namespace te
{
    /** Bullet implementation of a FSoftBody. */
    class BulletFSoftBody : public FSoftBody
    {
    public:
        BulletFSoftBody(BulletSoftBody* body, btSoftBody* btBody = nullptr);

        /** Set current btSoftBody */
        void SetBtSoftBody(btSoftBody* body) { _btSoftBody = body; }

        /** Return current btSoftBody */
        const auto& GetBtSoftBody() const { return _btSoftBody; }

        /** Set current BulletSoftBody */
        void SetSoftBody(BulletSoftBody* body) { _softBody = body; }

        /** Return current BulletSoftBody */
        const auto& GetSoftBody() const { return _softBody; }

        /** @copydoc FSoftBody::SetScale */
        void SetScale(const Vector3& scale) override;

        /** @copydoc FSoftBody::GetScale */
        const Vector3& GetScale() const override;

        /** @copydoc FSoftBody::GetPosition */
        Vector3 GetPosition() const override;

        /** @copydoc FSoftBody::GetRotation */
        Quaternion GetRotation() const override;

        /** @copydoc FSoftBody::GetBoundingBox */
        AABox GetBoundingBox() const override;

        /** @copydoc FSoftBody::SetTransform */
        void SetTransform(const Vector3& position, const Quaternion& rotation) override;

        /** @copydoc FSoftBody::SetIsTrigger */
        void SetIsTrigger(bool trigger) override;

        /** @copydoc FSoftBody::GetIsTrigger */
        bool GetIsTrigger() const override { return _isTrigger; }

        /** @copydoc FSoftBody::SetIsTrigger */
        void SetIsDebug(bool debug) override;

        /** @copydoc FSoftBody::GetIsTrigger */
        bool GetIsDebug() const override { return _isDebug; }

        /** @copydoc FSoftBody::SetMass */
        void SetMass(float mass) override;

        /** @copydoc FSoftBody::GetMass */
        float GetMass() const override { return _mass; }

        /** @copydoc FSoftBody::SetIsKinematic */
        void SetIsKinematic(bool kinematic) override;

        /** @copydoc FSoftBody::GetIsKinematic */
        bool GetIsKinematic() const override { return _isKinematic; }

        /** @copydoc FSoftBody::SetVelocity */
        void SetVelocity(const Vector3& velocity) override;

        /** @copydoc FSoftBody::GetVelocity */
        const Vector3& GetVelocity() const override { return _velocity; }

        /** @copydoc FSoftBody::SetAngularVelocity */
        void SetAngularVelocity(const Vector3& velocity) override;

        /** @copydoc FSoftBody::GetAngularVelocity */
        const Vector3& GetAngularVelocity() const override { return _angularVelocity; }

        /** @copydoc FSoftBody::SetFriction */
        void SetFriction(float friction) override;

        /** @copydoc FSoftBody::GetFriction */
        float GetFriction() const override { return _friction; }

        /** @copydoc FSoftBody::SetRollingFriction */
        void SetRollingFriction(float rollingFriction) override;

        /** @copydoc FSoftBody::GetRollingFriction */
        float GetRollingFriction() const override { return _rollingFriction; }

        /** @copydoc FSoftBody::SetRestitution */
        void SetRestitution(float restitution) override;

        /** @copydoc FSoftBody::GetRestitution */
        float GetRestitution() const override { return _restitution; }

        /** @copydoc FSoftBody::ApplyForce */
        void ApplyForce(const Vector3& force, ForceMode mode) const override;

        /** @copydoc FSoftBody::ApplyForceAtPoint */
        void ApplyForceAtPoint(const Vector3& force, const Vector3& position, ForceMode mode) const override;

        /** @copydoc FSoftBody::ApplyTorque */
        void ApplyTorque(const Vector3& torque, ForceMode mode) const override;

        /** @copydoc FSoftBody::SetCollisionReportMode */
        void SetCollisionReportMode(CollisionReportMode mode) override;

        /** @copydoc FSoftBody::GetCollisionReportMode */
        CollisionReportMode GetCollisionReportMode() const override { return _collisionReportMode; }

        /** @copydoc FSoftBody::SetFlags */
        void SetFlags(BodyFlag flags) override;

        /** @copydoc FSoftBody::GetFlags */
        BodyFlag GetFlags() const override { return _flags; }

    protected:
        friend class BulletMeshSoftBody;
        friend class BulletEllipsoidSoftBody;
        friend class BulletRopeSoftBody;
        friend class BulletPatchSoftBody;

    protected:
        BulletSoftBody* _softBody;
        btSoftBody* _btSoftBody;

        float _mass = 0.0f;
        float _friction = 0.0f;
        float _rollingFriction = 0.0f;
        float _restitution = 0.0f;
        bool _isKinematic = false;
        bool _isTrigger = false;
        bool _isDebug = true;

        Vector3 _scale = Vector3::ONE;
        Vector3 _position = Vector3::ZERO;
        Vector3 _velocity = Vector3::ZERO;
        Vector3 _angularVelocity = Vector3::ZERO;
        Quaternion _rotation = Quaternion::IDENTITY;

        BodyFlag _flags = (BodyFlag)((UINT32)BodyFlag::None);
        CollisionReportMode _collisionReportMode = CollisionReportMode::None;
    };
}
