#include "TeBulletConeTwistJoint.h"
#include "TeBulletPhysics.h"

namespace te
{ 
    BulletConeTwistJoint::BulletConeTwistJoint(BulletPhysics* physics, const CONE_TWIST_JOINT_DESC& desc)
        : ConeTwistJoint(desc)
    { }

    BulletConeTwistJoint::~BulletConeTwistJoint()
    { }
}
