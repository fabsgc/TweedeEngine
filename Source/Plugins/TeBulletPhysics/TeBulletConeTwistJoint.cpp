#include "TeBulletConeTwistJoint.h"
#include "TeBulletPhysics.h"
#include "TeBulletFJoint.h"

namespace te
{ 
    BulletConeTwistJoint::BulletConeTwistJoint(BulletPhysics* physics, BulletScene* scene)
        : ConeTwistJoint()
        , BulletJoint(physics, scene, this)
    {
        _internal = te_new<BulletFJoint>(physics, scene, this);
    }

    BulletConeTwistJoint::~BulletConeTwistJoint()
    {
        te_delete((BulletFJoint*)_internal);
    }

    void BulletConeTwistJoint::BuildJoint()
    {
        ReleaseJoint();
    }

    void BulletConeTwistJoint::UpdateJoint()
    {

    }

    void BulletConeTwistJoint::ReleaseJoint()
    {

    }
}
