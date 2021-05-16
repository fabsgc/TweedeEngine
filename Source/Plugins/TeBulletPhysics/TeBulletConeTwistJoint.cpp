#include "TeBulletConeTwistJoint.h"
#include "TeBulletPhysics.h"

namespace te
{ 
    BulletConeTwistJoint::BulletConeTwistJoint(BulletPhysics* physics, BulletScene* scene, const CONE_TWIST_JOINT_DESC& desc)
        : ConeTwistJoint(desc)
        , BulletJoint(physics, scene)
        , _joint(nullptr)
    {

    }

    BulletConeTwistJoint::~BulletConeTwistJoint()
    {

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
