#include "TeBulletFJoint.h"
#include "TeBulletRigidBody.h"
#include "Physics/TeJoint.h"

namespace te
{
    BulletFJoint::BulletFJoint(BulletPhysics* physics, BulletScene* scene, const JOINT_DESC& desc)
        : FJoint(desc)
    { }

    BulletFJoint::~BulletFJoint()
    { }
}
