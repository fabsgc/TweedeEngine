#include "TeBulletSliderJoint.h"
#include "TeBulletPhysics.h"

namespace te
{
    BulletSliderJoint::BulletSliderJoint(BulletPhysics* physics, const SLIDER_JOINT_DESC& desc)
        : SliderJoint(desc)
    { }

    BulletSliderJoint::~BulletSliderJoint()
    { }
}
