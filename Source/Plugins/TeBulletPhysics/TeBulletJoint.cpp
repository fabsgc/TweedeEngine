#include "TeBulletJoint.h"

namespace te
{
    BulletJoint::BulletJoint(BulletPhysics* physics, BulletScene* scene)
        : _physics(physics)
        , _scene(scene)
        , _joint(nullptr)
    { }

    BulletJoint::~BulletJoint()
    { }
}
