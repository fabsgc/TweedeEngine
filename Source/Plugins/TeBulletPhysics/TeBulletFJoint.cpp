#include "TeBulletFJoint.h"
#include "TeBulletRigidBody.h"
#include "TeBulletJoint.h"
#include "Physics/TeJoint.h"

namespace te
{
    BulletFJoint::BulletFJoint(BulletPhysics* physics, BulletScene* scene, BulletJoint* parent)
        : FJoint()
        , _physics(physics)
        , _scene(scene)
        , _parent(parent)
    { }

    BulletFJoint::BulletFJoint()
        : FJoint()
    { }

    BulletFJoint::~BulletFJoint()
    { }
}
