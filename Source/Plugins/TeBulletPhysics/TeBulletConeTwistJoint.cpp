#include "TeBulletConeTwistJoint.h"
#include "TeBulletPhysics.h"
#include "TeBulletFJoint.h"

namespace te
{ 
    BulletConeTwistJoint::BulletConeTwistJoint(BulletPhysics* physics, BulletScene* scene, const CONE_TWIST_JOINT_DESC& desc)
        : ConeTwistJoint(desc)
        , BulletJoint(physics, scene)
        , _joint(nullptr)
    {
        _internal = te_new<BulletFJoint>(physics, scene, this, desc);
    }

    BulletConeTwistJoint::~BulletConeTwistJoint()
    {
        te_delete((BulletFJoint*)_internal);
    }

    void BulletConeTwistJoint::BuildJoint()
    {
        ReleaseJoint();
    }

    void BulletConeTwistJoint::UpdateJoint()
    {

    }

    void BulletConeTwistJoint::ReleaseJoint()
    {

    }
}
