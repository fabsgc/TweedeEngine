#include "TeBulletHingeJoint.h"
#include "TeBulletPhysics.h"
#include "TeBulletFJoint.h"

namespace te
{
    BulletHingeJoint::BulletHingeJoint(BulletPhysics* physics, BulletScene* scene, const HINGE_JOINT_DESC& desc)
        : HingeJoint(desc)
        , BulletJoint(physics, scene)
        , _joint(nullptr)
    {
        _internal = te_new<BulletFJoint>(physics, scene, this, desc);
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
