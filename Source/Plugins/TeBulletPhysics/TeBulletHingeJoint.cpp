#include "TeBulletHingeJoint.h"
#include "TeBulletPhysics.h"

namespace te
{
    BulletHingeJoint::BulletHingeJoint(BulletPhysics* physics, const HINGE_JOINT_DESC& desc)
        : HingeJoint(desc)
    { }

    BulletHingeJoint::~BulletHingeJoint()
    { }
}
