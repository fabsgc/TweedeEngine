#include "TeBulletSphericalJoint.h"
#include "TeBulletPhysics.h"
#include "TeBulletFJoint.h"

namespace te
{
    BulletSphericalJoint::BulletSphericalJoint(BulletPhysics* physics, BulletScene* scene)
        : SphericalJoint()
        , BulletJoint(physics, scene)
    {
        _internal = te_new<BulletFJoint>(physics, scene, this);
    }

    BulletSphericalJoint::~BulletSphericalJoint()
    {
        te_delete((BulletFJoint*)_internal);
    }

    void BulletSphericalJoint::SetBody(JointBody body, Body* value)
    {
        Joint::SetBody(body, value);
        BuildJoint();
    }

    void BulletSphericalJoint::SetTransform(JointBody body, const Vector3& position, const Quaternion& rotation)
    {
        Joint::SetTransform(body, position, rotation);
        UpdateJoint();
    }

    void BulletSphericalJoint::SetBreakForce(float force)
    {
        Joint::SetBreakForce(force);
        UpdateJoint();
    }

    void BulletSphericalJoint::SetBreakTorque(float torque)
    {
        Joint::SetBreakTorque(torque);
        UpdateJoint();
    }

    void BulletSphericalJoint::SetEnableCollision(bool value)
    {
        Joint::SetEnableCollision(value);
        UpdateJoint();
    }

    void BulletSphericalJoint::BuildJoint()
    {
        ReleaseJoint();
    }

    void BulletSphericalJoint::UpdateJoint()
    {

    }

    void BulletSphericalJoint::ReleaseJoint()
    {

    }
}
