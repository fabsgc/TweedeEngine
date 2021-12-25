#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TePhysicsCommon.h"
#include "Physics/TeBody.h"

namespace te
{
    /**
     * SoftBody is a dynamic physics object that can be moved using forces (or directly). It will interact with other
     * static and dynamic physics objects in the scene accordingly (it will push other non-kinematic rigidbodies,
     * and collide with static objects).
     */
    class TE_CORE_EXPORT SoftBody : public Body
    {
    public:
        /**
         * We can scale the PhysicsMesh in order to match to the 3D mesh geometry
         */
        void SetScale(const Vector3& scale);

        /** @copydoc SetScale */
        const Vector3& GetScale() const;

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
        bool GetIsTrigger() const override;

        /** @copydoc FSoftBody::SetIsTrigger */
        void SetIsDebug(bool debug) override;

        /** @copydoc FSoftBody::GetIsTrigger */
        bool GetIsDebug() const override;

        /** @copydoc FSoftBody::SetMass */
        void SetMass(float mass) override;

        /** @copydoc FSoftBody::GetMass */
        float GetMass() const override;

        /** @copydoc FSoftBody::SetIsKinematic */
        void SetIsKinematic(bool kinematic) override;

        /** @copydoc FSoftBody::GetIsKinematic */
        bool GetIsKinematic() const override;

        /** @copydoc FSoftBody::SetVelocity */
        void SetVelocity(const Vector3& velocity) override;

        /** @copydoc FSoftBody::GetVelocity */
        const Vector3& GetVelocity() const override;

        /** @copydoc FSoftBody::SetAngularVelocity */
        void SetAngularVelocity(const Vector3& velocity) override;

        /** @copydoc FSoftBody::GetAngularVelocity */
        const Vector3& GetAngularVelocity() const override;

        /** @copydoc FSoftBody::SetFriction */
        void SetFriction(float friction) override;

        /** @copydoc FSoftBody::GetFriction */
        float GetFriction() const override;

        /** @copydoc FSoftBody::SetRollingFriction */
        void SetRollingFriction(float rollingFriction) override;

        /** @copydoc FSoftBody::GetRollingFriction */
        float GetRollingFriction() const override;

        /** @copydoc FSoftBody::SetRestitution */
        void SetRestitution(float restitution) override;

        /** @copydoc FSoftBody::GetRestitution */
        float GetRestitution() const override;

        /** @copydoc FSoftBody::ApplyForce */
        void ApplyForce(const Vector3& force, ForceMode mode) const override;

        /** @copydoc FSoftBody::ApplyForceAtPoint */
        void ApplyForceAtPoint(const Vector3& force, const Vector3& position, ForceMode mode) const override;

        /** @copydoc FSoftBody::ApplyTorque */
        void ApplyTorque(const Vector3& torque, ForceMode mode) const override;

        /** @copydoc FSoftBody::SetCollisionReportMode */
        void SetCollisionReportMode(CollisionReportMode mode) override;

        /** @copydoc FSoftBody::GetCollisionReportMode */
        CollisionReportMode GetCollisionReportMode() const override;

        /** @copydoc FSoftBody::SetFlags */
        void SetFlags(BodyFlag flags) override;

        /** @copydoc FSoftBody::GetFlags */
        BodyFlag GetFlags() const override;

    protected:
        /**
         * Constructs a new SoftBody.
         *
         * @param[in]	linkedSO	Scene object that owns this softbody. All physics updates applied to this object
         *							will be transfered to this scene object (the movement/rotation resulting from
         *							those updates).
         * @param[in]   type        TypeID_Core type
         */
        SoftBody(const HSceneObject& linkedSO, UINT32 type);
    };
}
