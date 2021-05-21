#include "TeBulletJoint.h"
#include "TeBulletFBody.h"
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

    btRigidBody* BulletJoint::GetBtRigidBody(BodyInfo* info)
    {
        RigidBody* body = info->BodyElt;
        BulletFBody* fBody = nullptr;

        if (body)
        {
            fBody = static_cast<BulletFBody*>(body->GetInternal());
            if (fBody)
                return static_cast<btRigidBody*>(fBody->GetBody());
        }

        return nullptr;
    }
}
