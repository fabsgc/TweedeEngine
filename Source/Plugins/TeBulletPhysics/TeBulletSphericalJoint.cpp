#include "TeBulletSphericalJoint.h"
#include "TeBulletPhysics.h"
#include "TeBulletFJoint.h"

namespace te
{
    BulletSphericalJoint::BulletSphericalJoint(BulletPhysics* physics, BulletScene* scene, const SPHERICAL_JOINT_DESC& desc)
        : SphericalJoint(desc)
        , BulletJoint(physics, scene)
        , _joint(nullptr)
    {
        _internal = te_new<BulletFJoint>(physics, scene, this, desc);
    }

    BulletSphericalJoint::~BulletSphericalJoint()
    {
        te_delete((BulletFJoint*)_internal);
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
