#include "TeBulletSliderJoint.h"
#include "TeBulletPhysics.h"
#include "TeBulletFJoint.h"

namespace te
{
    BulletSliderJoint::BulletSliderJoint(BulletPhysics* physics, BulletScene* scene, const SLIDER_JOINT_DESC& desc)
        : SliderJoint(desc)
        , _joint(nullptr)
    {
        _internal = te_new<BulletFJoint>(physics, scene, desc);
    }

    BulletSliderJoint::~BulletSliderJoint()
    {
        te_delete((BulletFJoint*)_internal);
    }
}
