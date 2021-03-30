#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeD6Joint.h"

namespace te
{
    /** Bullet implementation of a D6 joint. */
    class BulletD6Joint : public D6Joint
    {
    public:
        BulletD6Joint(BulletPhysics* physics, const D6_JOINT_DESC& desc);
        ~BulletD6Joint();
    };
}
