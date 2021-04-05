#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeFJoint.h"

namespace te
{
    /** Bullet implementation of an FJoint. */
    class BulletFJoint : public FJoint
    {
    public:
        BulletFJoint(BulletPhysics* physics, BulletScene* scene, const JOINT_DESC& desc);
        ~BulletFJoint();
    };
}
