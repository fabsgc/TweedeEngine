#include "TeBulletConeTwistJoint.h"
#include "TeBulletPhysics.h"
#include "TeBulletFJoint.h"

namespace te
{ 
    BulletConeTwistJoint::BulletConeTwistJoint(BulletPhysics* physics, BulletScene* scene, const CONE_TWIST_JOINT_DESC& desc)
        : ConeTwistJoint(desc)
    {
        _internal = te_new<BulletFJoint>(physics, scene, desc);
    }

    BulletConeTwistJoint::~BulletConeTwistJoint()
    {
        te_delete((BulletFJoint*)_internal);
    }
}
