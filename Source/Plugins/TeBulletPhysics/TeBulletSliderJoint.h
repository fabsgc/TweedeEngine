#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeSliderJoint.h"

namespace te
{
    /** Bullet implementation of a Slider joint. */
    class BulletSliderJoint : public SliderJoint
    {
    public:
        BulletSliderJoint(BulletPhysics* physics, const SLIDER_JOINT_DESC& desc);
        ~BulletSliderJoint();
    };
}
