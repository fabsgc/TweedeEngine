#include "TeBulletD6Joint.h"
#include "TeBulletPhysics.h"

namespace te
{
    BulletD6Joint::BulletD6Joint(BulletPhysics* physics, BulletScene* scene)
        : D6Joint()
        , BulletJoint(physics, scene)
    {

    }

    BulletD6Joint::~BulletD6Joint()
    {

    }

    void BulletD6Joint::BuildJoint()
    {
        ReleaseJoint();
    }

    void BulletD6Joint::UpdateJoint()
    {

    }

    void BulletD6Joint::ReleaseJoint()
    {

    }
}
