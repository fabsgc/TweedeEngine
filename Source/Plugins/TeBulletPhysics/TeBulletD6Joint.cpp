#include "TeBulletD6Joint.h"
#include "TeBulletPhysics.h"

namespace te
{
    BulletD6Joint::BulletD6Joint(BulletPhysics* physics, const D6_JOINT_DESC& desc)
        : D6Joint(desc)
    { }

    BulletD6Joint::~BulletD6Joint()
    { }
}
