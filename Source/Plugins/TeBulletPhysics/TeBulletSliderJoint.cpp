#include "TeBulletSliderJoint.h"
#include "TeBulletPhysics.h"
#include "TeBulletFJoint.h"

namespace te
{
    BulletSliderJoint::BulletSliderJoint(BulletPhysics* physics, BulletScene* scene)
        : SliderJoint()
        , BulletJoint(physics, scene, this)
    {
        _internal = te_new<BulletFJoint>(physics, scene, this);
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
