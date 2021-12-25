#include "Physics/TeSoftBody.h"
#include "Physics/TePhysics.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"
#include "TeFSoftBody.h"
#include "Math/TeAABox.h"

namespace te
{ 
    SoftBody::SoftBody(const HSceneObject& linkedSO)
        : Body(linkedSO, TypeID_Core::TID_SoftBody)
    { }

    void SoftBody::SetScale(const Vector3& scale)
    {
        static_cast<FSoftBody*>(_internal)->SetScale(scale);
    }

    const Vector3& SoftBody::GetScale() const
    {
        return static_cast<FSoftBody*>(_internal)->GetScale();
    }

    Vector3 SoftBody::GetPosition() const
    {
        return static_cast<FSoftBody*>(_internal)->GetPosition();
    }

    Quaternion SoftBody::GetRotation() const
    {
        return static_cast<FSoftBody*>(_internal)->GetRotation();
    }

    AABox SoftBody::GetBoundingBox() const
    {
        return static_cast<FSoftBody*>(_internal)->GetBoundingBox();
    }

    void SoftBody::SetTransform(const Vector3& position, const Quaternion& rotation)
    {
        static_cast<FSoftBody*>(_internal)->SetTransform(position, rotation);
    }

    void SoftBody::SetIsTrigger(bool trigger)
    {
        static_cast<FSoftBody*>(_internal)->SetIsTrigger(trigger);
    }

    bool SoftBody::GetIsTrigger() const
    {
        return static_cast<FSoftBody*>(_internal)->GetIsTrigger();
    }

    void SoftBody::SetIsDebug(bool debug)
    {
        static_cast<FSoftBody*>(_internal)->SetIsDebug(debug);
    }

    bool SoftBody::GetIsDebug() const
    {
        return static_cast<FSoftBody*>(_internal)->GetIsDebug();
    }

    void SoftBody::SetMass(float mass)
    {
        static_cast<FSoftBody*>(_internal)->SetMass(mass);
    }

    float SoftBody::GetMass() const
    {
        return static_cast<FSoftBody*>(_internal)->GetMass();
    }

    void SoftBody::SetIsKinematic(bool kinematic)
    {
        static_cast<FSoftBody*>(_internal)->SetIsKinematic(kinematic);
    }

    bool SoftBody::GetIsKinematic() const
    {
        return static_cast<FSoftBody*>(_internal)->GetIsKinematic();
    }

    void SoftBody::SetVelocity(const Vector3& velocity)
    {
        static_cast<FSoftBody*>(_internal)->SetVelocity(velocity);
    }

    const Vector3& SoftBody::GetVelocity() const
    {
        return static_cast<FSoftBody*>(_internal)->GetVelocity();
    }

    void SoftBody::SetAngularVelocity(const Vector3& velocity)
    {
        static_cast<FSoftBody*>(_internal)->SetAngularVelocity(velocity);
    }

    const Vector3& SoftBody::GetAngularVelocity() const
    {
        return static_cast<FSoftBody*>(_internal)->GetAngularVelocity();
    }

    void SoftBody::SetFriction(float friction)
    {
        static_cast<FSoftBody*>(_internal)->SetFriction(friction);
    }

    float SoftBody::GetFriction() const
    {
        return static_cast<FSoftBody*>(_internal)->GetFriction();
    }

    void SoftBody::SetRollingFriction(float rollingFriction)
    {
        static_cast<FSoftBody*>(_internal)->SetRollingFriction(rollingFriction);
    }

    float SoftBody::GetRollingFriction() const
    {
        return static_cast<FSoftBody*>(_internal)->GetRollingFriction();
    }

    void SoftBody::SetRestitution(float restitution)
    {
        static_cast<FSoftBody*>(_internal)->SetRestitution(restitution);
    }

    float SoftBody::GetRestitution() const
    {
        return static_cast<FSoftBody*>(_internal)->GetRestitution();
    }

    void SoftBody::SetFlags(BodyFlag flags)
    {
        static_cast<FSoftBody*>(_internal)->SetFlags(flags);
    }

    BodyFlag SoftBody::GetFlags() const
    {
        return static_cast<FSoftBody*>(_internal)->GetFlags();
    }

    void SoftBody::ApplyForce(const Vector3& force, ForceMode mode) const
    {
        static_cast<FSoftBody*>(_internal)->ApplyForce(force, mode);
    }

    void SoftBody::ApplyForceAtPoint(const Vector3& force, const Vector3& position, ForceMode mode) const
    {
        static_cast<FSoftBody*>(_internal)->ApplyForceAtPoint(force, position, mode);
    }

    void SoftBody::ApplyTorque(const Vector3& torque, ForceMode mode) const
    {
        static_cast<FSoftBody*>(_internal)->ApplyTorque(torque, mode);
    }

    void SoftBody::SetCollisionReportMode(CollisionReportMode mode)
    {
        static_cast<FSoftBody*>(_internal)->SetCollisionReportMode(mode);
    }

    CollisionReportMode SoftBody::GetCollisionReportMode() const
    {
        return static_cast<FSoftBody*>(_internal)->GetCollisionReportMode();
    }
}
