#include "TeBulletD6Joint.h"
#include "TeBulletPhysics.h"
#include "TeBulletFJoint.h"

namespace te
{
    BulletD6Joint::BulletD6Joint(BulletPhysics* physics, BulletScene* scene)
        : D6Joint()
        , BulletJoint(physics, scene, this)
    {
        _internal = te_new<BulletFJoint>(physics, scene, this);
    }

    BulletD6Joint::~BulletD6Joint()
    {
        te_delete((BulletFJoint*)_internal);
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
