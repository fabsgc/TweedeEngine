#include "TeBulletFJoint.h"
#include "TeBulletRigidBody.h"
#include "Physics/TeJoint.h"

namespace te
{
    BulletFJoint::BulletFJoint(BulletPhysics* physics, BulletScene* scene, const JOINT_DESC& desc)
        : FJoint(desc)
        , _physics(physics)
        , _scene(scene)
        , _joint()
    { }

    BulletFJoint::~BulletFJoint()
    { }

    Body* BulletFJoint::GetBody(JointBody body) const
    {
        return nullptr; // TODO
    }

    void BulletFJoint::SetBody(JointBody body, Body* value)
    {
        // TODO
    }

    Vector3 BulletFJoint::GetPosition(JointBody body) const
    {
        return Vector3::ZERO; // TODO
    }

    Quaternion BulletFJoint::GetRotation(JointBody body) const
    {
        return Quaternion::ZERO; // TODO;
    }

    void BulletFJoint::SetTransform(JointBody body, const Vector3& position, const Quaternion& rotation)
    {
        // TODO
    }

    float BulletFJoint::GetBreakForce() const
    {
        return 0.0f; // TODO
    }

    void BulletFJoint::SetBreakForce(float force)
    {
        // TODO
    }

    float BulletFJoint::GetBreakTorque() const
    {
        return 0.0f; // TODO
    }

    void BulletFJoint::SetBreakTorque(float torque)
    {
        // TODO
    }

    bool BulletFJoint::GetEnableCollision() const
    {
        return false; // TODO
    }

    void BulletFJoint::SetEnableCollision(bool value)
    {
        // TODO
    }
}
