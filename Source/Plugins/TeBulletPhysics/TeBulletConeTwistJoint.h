#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeConeTwistJoint.h"

namespace te
{
    /** Bullet implementation of a ConeTwist joint. */
    class BulletConeTwistJoint : public ConeTwistJoint
    {
    public:
        BulletConeTwistJoint(BulletPhysics* physics, const CONE_TWIST_JOINT_DESC& desc);
        ~BulletConeTwistJoint();
    };
}
