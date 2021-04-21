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
        te_delete((BulletFBody*)_internal);
    }

    Vector3 BulletSoftBody::GetPosition() const
    {
        return _position;
    }

    Quaternion BulletSoftBody::GetRotation() const
    {
        return _rotation;
    }

    void BulletSoftBody::SetTransform(const Vector3& pos, const Quaternion& rot, bool activate)
    {
        // TODO
    }

    void BulletSoftBody::SetIsTrigger(bool value)
    {
        // TODO
    }

    void BulletSoftBody::SetIsDebug(bool debug)
    {
        // TODO
    }

    void BulletSoftBody::SetMass(float mass)
    {
        // TODO
    }

    void BulletSoftBody::SetIsKinematic(bool kinematic)
    {
        // TODO
    }

    void BulletSoftBody::SetVelocity(const Vector3& velocity)
    {
        // TODO
    }

    void BulletSoftBody::SetAngularVelocity(const Vector3& velocity)
    {
        // TODO
    }

    void BulletSoftBody::SetFriction(float friction)
    {
        // TODO
    }

    void BulletSoftBody::SetRollingFriction(float rollingFriction)
    {
        // TODO
    }

    void BulletSoftBody::SetRestitution(float restitution)
    {
        // TODO
    }

    void BulletSoftBody::SetUseGravity(bool gravity)
    {
        // TODO
    }

    void BulletSoftBody::SetCenterOfMass(const Vector3& centerOfMass)
    {
        // TODO
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

    void BulletSoftBody::SetCollisionReportMode(CollisionReportMode mode)
    {
        // TODO
    }

    void BulletSoftBody::AddCollider(Collider* collider)
    {
        // TODO
    }

    void BulletSoftBody::SyncCollider(Collider* collider)
    {
        // TODO
    }

    void BulletSoftBody::RemoveCollider(Collider* collider)
    {
        // TODO
    }

    void BulletSoftBody::RemoveColliders()
    {
        // TODO
    }

    void BulletSoftBody::SetFlags(BodyFlag flags)
    {
        // TODO
    }
}
