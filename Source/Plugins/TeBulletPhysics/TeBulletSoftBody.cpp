#include "TeBulletSoftBody.h"
#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"
#include "TeBulletPhysics.h"
#include "TeBulletFBody.h"

namespace te
{
    BulletSoftBody::BulletSoftBody(BulletPhysics* physics, BulletScene* scene, const HSceneObject& linkedSO)
        : SoftBody(linkedSO)
    { 
        _internal = te_new<BulletFBody>(physics, scene);
    }

    BulletSoftBody::~BulletSoftBody()
    { 
        te_delete(_internal);  // TODO
    }

    void BulletSoftBody::Move(const Vector3& position)
    {
        // TODO
    }

    void BulletSoftBody::Rotate(const Quaternion& rotation)
    {
        // TODO
    }

    Vector3 BulletSoftBody::GetPosition() const
    {
        return _position;
    }

    Quaternion BulletSoftBody::GetRotation() const
    {
        return _rotation;
    }

    void BulletSoftBody::SetTransform(const Vector3& pos, const Quaternion& rot)
    {
        // TODO
    }

    void BulletSoftBody::SetMass(float mass)
    {
        // TODO
    }

    float BulletSoftBody::GetMass() const
    {
        return 0.0f; // TODO
    }

    void BulletSoftBody::SetIsKinematic(bool kinematic)
    {
        // TODO
    }

    bool BulletSoftBody::GetIsKinematic() const
    {
        return false; // TODO
    }

    void BulletSoftBody::SetVelocity(const Vector3& velocity)
    {
        // TODO
    }

    const Vector3& BulletSoftBody::GetVelocity() const
    {
        return Vector3::ZERO;
    }

    void BulletSoftBody::SetAngularVelocity(const Vector3& velocity)
    {
        // TODO
    }

    const Vector3& BulletSoftBody::GetAngularVelocity() const
    {
        return Vector3::ZERO;
    }

    void BulletSoftBody::SetFriction(float friction)
    {
        // TODO
    }

    float BulletSoftBody::GetFriction() const
    {
        return 0.0f; // TODO
    }

    void BulletSoftBody::SetRollingFriction(float rollingFriction)
    {
        // TODO
    }

    float BulletSoftBody::GetRollingFriction() const
    {
        return 0.0f; // TODO
    }

    void BulletSoftBody::SetRestitution(float restitution)
    {
        // TODO
    }

    float BulletSoftBody::GetRestitution() const
    {
        return 0.0f; // TODO
    }

    void BulletSoftBody::SetUseGravity(bool gravity)
    {
        // TODO
    }

    bool BulletSoftBody::GetUseGravity() const
    {
        return false;
    }

    void BulletSoftBody::SetCenterOfMass(const Vector3& centerOfMass)
    {
        // TODO
    }

    const Vector3& BulletSoftBody::GetCenterOfMass() const
    {
        return _centerOfMass;
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

    void BulletSoftBody::AddCollider(Collider* collider)
    {
        TE_PRINT("Add Collider");
    }

    void BulletSoftBody::RemoveCollider(Collider* collider)
    {
        TE_PRINT("Remove Collider");
    }

    void BulletSoftBody::RemoveColliders()
    {
        TE_PRINT("Remove all Colliders");
    }

    void BulletSoftBody::SetFlags(BodyFlag flags)
    {
        // TODO
    }
}
