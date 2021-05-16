#include "TeBulletHingeJoint.h"
#include "TeBulletPhysics.h"

namespace te
{
    BulletHingeJoint::BulletHingeJoint(BulletPhysics* physics, BulletScene* scene, const HINGE_JOINT_DESC& desc)
        : HingeJoint(desc)
        , BulletJoint(physics, scene)
        , _joint(nullptr)
    {

    }

    BulletHingeJoint::~BulletHingeJoint()
    {

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
