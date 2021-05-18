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
