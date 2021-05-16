#include "TeBulletD6Joint.h"
#include "TeBulletPhysics.h"
#include "TeBulletFJoint.h"

namespace te
{
    BulletD6Joint::BulletD6Joint(BulletPhysics* physics, BulletScene* scene, const D6_JOINT_DESC& desc)
        : D6Joint(desc)
        , BulletJoint(physics, scene)
    { 
        _internal = te_new<BulletFJoint>(physics, scene, this, desc);
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
