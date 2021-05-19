#include "TeBulletHingeJoint.h"
#include "TeBulletPhysics.h"
#include "TeBulletFJoint.h"

namespace te
{
    BulletHingeJoint::BulletHingeJoint(BulletPhysics* physics, BulletScene* scene)
        : HingeJoint()
        , BulletJoint(physics, scene, this)
    {
        _internal = te_new<BulletFJoint>(physics, scene, this);
    }

    BulletHingeJoint::~BulletHingeJoint()
    {
        te_delete((BulletFJoint*)_internal);
    }

    void BulletHingeJoint::BuildJoint()
    {
        ReleaseJoint();
    }

    void BulletHingeJoint::UpdateJoint()
    {

    }

    void BulletHingeJoint::ReleaseJoint()
    {

    }
}
