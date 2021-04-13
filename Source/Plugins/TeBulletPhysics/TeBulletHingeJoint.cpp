#include "TeBulletHingeJoint.h"
#include "TeBulletPhysics.h"
#include "TeBulletFJoint.h"

namespace te
{
    BulletHingeJoint::BulletHingeJoint(BulletPhysics* physics, BulletScene* scene, const HINGE_JOINT_DESC& desc)
        : HingeJoint(desc)
    {
        _internal = te_new<BulletFJoint>(physics, scene, desc);
    }

    BulletHingeJoint::~BulletHingeJoint()
    {
        te_delete((BulletFJoint*)_internal);
    }
}
