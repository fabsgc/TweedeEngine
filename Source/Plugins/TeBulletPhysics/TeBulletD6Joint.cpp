#include "TeBulletD6Joint.h"
#include "TeBulletPhysics.h"
#include "TeBulletFJoint.h"

namespace te
{
    BulletD6Joint::BulletD6Joint(BulletPhysics* physics, BulletScene* scene, const D6_JOINT_DESC& desc)
        : D6Joint(desc)
    { 
        _internal = te_new<BulletFJoint>(physics, scene, desc);
    }

    BulletD6Joint::~BulletD6Joint()
    { 
        te_delete((BulletFJoint*)_internal);
    }
}
