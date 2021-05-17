#include "TeBulletHingeJoint.h"
#include "TeBulletPhysics.h"

namespace te
{
    BulletHingeJoint::BulletHingeJoint(BulletPhysics* physics, BulletScene* scene)
        : HingeJoint()
        , BulletJoint(physics, scene)
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
