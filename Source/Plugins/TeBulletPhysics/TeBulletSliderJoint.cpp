#include "TeBulletSliderJoint.h"
#include "TeBulletPhysics.h"

namespace te
{
    BulletSliderJoint::BulletSliderJoint(BulletPhysics* physics, BulletScene* scene, const SLIDER_JOINT_DESC& desc)
        : SliderJoint(desc)
        , BulletJoint(physics, scene)
        , _joint(nullptr)
    {

    }

    BulletSliderJoint::~BulletSliderJoint()
    {

    }

    void BulletSliderJoint::BuildJoint()
    {
        ReleaseJoint();
    }

    void BulletSliderJoint::UpdateJoint()
    {

    }

    void BulletSliderJoint::ReleaseJoint()
    {

    }
}
