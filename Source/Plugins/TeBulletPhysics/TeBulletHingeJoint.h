#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeHingeJoint.h"

namespace te
{
    /** Bullet implementation of a Hinge joint. */
    class BulletHingeJoint : public HingeJoint
    {
    public:
        BulletHingeJoint(BulletPhysics* physics, BulletScene* scene, const HINGE_JOINT_DESC& desc);
        ~BulletHingeJoint();
    };
}
