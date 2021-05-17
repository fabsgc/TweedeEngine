#include "TeBulletConeTwistJoint.h"
#include "TeBulletPhysics.h"

namespace te
{ 
    BulletConeTwistJoint::BulletConeTwistJoint(BulletPhysics* physics, BulletScene* scene)
        : ConeTwistJoint()
        , BulletJoint(physics, scene)
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
