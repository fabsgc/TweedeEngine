#include "TeBulletSliderJoint.h"
#include "TeBulletPhysics.h"
#include "TeBulletFJoint.h"

namespace te
{
    BulletSliderJoint::BulletSliderJoint(BulletPhysics* physics, BulletScene* scene, const SLIDER_JOINT_DESC& desc)
        : SliderJoint(desc)
        , BulletJoint(physics, scene)
        , _joint(nullptr)
    {
        _internal = te_new<BulletFJoint>(physics, scene, this, desc);
    }

    BulletSliderJoint::~BulletSliderJoint()
    {
        te_delete((BulletFJoint*)_internal);
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
