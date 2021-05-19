#include "TeBulletJoint.h"
#include "Physics/TeRigidBody.h"

namespace te
{
    BulletJoint::BulletJoint(BulletPhysics* physics, BulletScene* scene, Joint* joint)
        : _physics(physics)
        , _scene(scene)
        , _btJoint(nullptr)
        , _joint(joint)
    { }

    BulletJoint::~BulletJoint()
    {
        BulletRigidBody* anchorBody = (BulletRigidBody*)_joint->GetBody(JointBody::Anchor);
        BulletRigidBody* targetBody = (BulletRigidBody*)_joint->GetBody(JointBody::Target);

        if(anchorBody)
            anchorBody->RemoveJoint(this);
        if(targetBody)
            targetBody->RemoveJoint(this);
    }
}
