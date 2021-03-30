#include "TeBulletSphericalJoint.h"
#include "TeBulletPhysics.h"

namespace te
{
    BulletSphericalJoint::BulletSphericalJoint(BulletPhysics* physics, const SPHERICAL_JOINT_DESC& desc)
        : SphericalJoint(desc)
    { }

    BulletSphericalJoint::~BulletSphericalJoint()
    { }
}
