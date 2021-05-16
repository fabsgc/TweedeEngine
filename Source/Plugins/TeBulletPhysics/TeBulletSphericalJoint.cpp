#include "TeBulletSphericalJoint.h"
#include "TeBulletPhysics.h"

namespace te
{
    BulletSphericalJoint::BulletSphericalJoint(BulletPhysics* physics, BulletScene* scene, const SPHERICAL_JOINT_DESC& desc)
        : SphericalJoint(desc)
        , BulletJoint(physics, scene)
        , _joint(nullptr)
    {

    }

    BulletSphericalJoint::~BulletSphericalJoint()
    {

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
