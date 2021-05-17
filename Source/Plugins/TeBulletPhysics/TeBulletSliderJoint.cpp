#include "TeBulletSliderJoint.h"
#include "TeBulletPhysics.h"

namespace te
{
    BulletSliderJoint::BulletSliderJoint(BulletPhysics* physics, BulletScene* scene)
        : SliderJoint()
        , BulletJoint(physics, scene)
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
