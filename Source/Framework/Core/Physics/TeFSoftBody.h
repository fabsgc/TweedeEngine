#pragma once

#include "Physics/TePhysicsCommon.h"
#include "Physics/TeBody.h"
#include "TeFBody.h"

namespace te
{
    /** Provides common functionality used by all SoftBody types. */
    class TE_CORE_EXPORT FSoftBody : public FBody
    {
    public:
        FSoftBody();

        /** @copydoc SoftBody::SetScale */
        virtual void SetScale(const Vector3& scale) = 0;

        /** @copydoc SoftBody::GetScale */
        virtual const Vector3& GetScale() const = 0;

        /** @copydoc Body::GetPosition */
        virtual Vector3 GetPosition() const = 0;

        /** @copydoc Body::GetRotation */
        virtual Quaternion GetRotation() const = 0;

        /** @copydoc Body::GetBoundingBox */
        virtual AABox GetBoundingBox() const = 0;

        /** @copydoc Body::SetTransform */
        virtual void SetTransform(const Vector3& position, const Quaternion& rotation) = 0;

        /** @copydoc Body::SetIsTrigger */
        virtual void SetIsTrigger(bool trigger) = 0;

        /** @copydoc Body::GetIsTrigger */
        virtual bool GetIsTrigger() const = 0;

        /** @copydoc Body::SetIsTrigger */
        virtual void SetIsDebug(bool debug) = 0;

        /** @copydoc Body::GetIsTrigger */
        virtual bool GetIsDebug() const = 0;

        /** @copydoc Body::SetMass */
        virtual void SetMass(float mass) = 0;

        /** @copydoc Body::GetMass */
        virtual float GetMass() const = 0;

        /** @copydoc Body::SetIsKinematic */
        virtual void SetIsKinematic(bool kinematic) = 0;

        /** @copydoc Body::GetIsKinematic */
        virtual bool GetIsKinematic() const = 0;

        /** @copydoc Body::SetVelocity */
        virtual void SetVelocity(const Vector3& velocity) = 0;

        /** @copydoc Body::GetVelocity */
        virtual const Vector3& GetVelocity() const = 0;

        /** @copydoc Body::SetAngularVelocity */
        virtual void SetAngularVelocity(const Vector3& velocity) = 0;

        /** @copydoc Body::GetAngularVelocity */
        virtual const Vector3& GetAngularVelocity() const = 0;

        /** @copydoc Body::SetFriction */
        virtual void SetFriction(float friction) = 0;

        /** @copydoc Body::GetFriction */
        virtual float GetFriction() const = 0;

        /** @copydoc Body::SetRollingFriction */
        virtual void SetRollingFriction(float rollingFriction) = 0;

        /** @copydoc Body::GetRollingFriction */
        virtual float GetRollingFriction() const = 0;

        /** @copydoc Body::SetRestitution */
        virtual void SetRestitution(float restitution) = 0;

        /** @copydoc Body::GetRestitution */
        virtual float GetRestitution() const = 0;

        /** @copydoc Body::ApplyForce */
        virtual void ApplyForce(const Vector3& force, ForceMode mode) const = 0;

        /** @copydoc Body::ApplyForceAtPoint */
        virtual void ApplyForceAtPoint(const Vector3& force, const Vector3& position, ForceMode mode) const = 0;

        /** @copydoc Body::ApplyTorque */
        virtual void ApplyTorque(const Vector3& torque, ForceMode mode) const = 0;

        /** @copydoc Body::SetCollisionReportMode */
        virtual void SetCollisionReportMode(CollisionReportMode mode) = 0;

        /** @copydoc Body::GetCollisionReportMode */
        virtual CollisionReportMode GetCollisionReportMode() const = 0;

        /** @copydoc Body::SetFlags */
        virtual void SetFlags(BodyFlag flags) = 0;

        /** @copydoc Body::GetFlags */
        virtual BodyFlag GetFlags() const = 0;
    };
}
