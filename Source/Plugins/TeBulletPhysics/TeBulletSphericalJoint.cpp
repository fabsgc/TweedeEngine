#include "TeBulletSphericalJoint.h"
#include "TeBulletPhysics.h"

namespace te
{
    BulletSphericalJoint::BulletSphericalJoint(BulletPhysics* physics, BulletScene* scene)
        : SphericalJoint()
        , BulletJoint(physics, scene)
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
