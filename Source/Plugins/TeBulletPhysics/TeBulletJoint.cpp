#include "TeBulletJoint.h"
#include "TeBulletFBody.h"
#include "Physics/TeRigidBody.h"

namespace te
{
    BulletJoint::BulletJoint(BulletPhysics* physics, BulletScene* scene, Joint* joint)
        : _physics(physics)
        , _scene(scene)
        , _btJoint(nullptr)
        , _btFeedBack(nullptr)
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

    Vector3 BulletJoint::GetAnchorScaledPosisition(RigidBody* bodyAnchor, BodyInfo* info, Vector3* offsetPivot)
    {
        return info->Position[(int)JointBody::Anchor] + offsetPivot[(int)JointBody::Anchor] - bodyAnchor->GetCenterOfMass();
    }

    Vector3 BulletJoint::GetTargetScaledPosisition(btRigidBody* btRigidBody, RigidBody* bodyTarget, BodyInfo* info, 
        Vector3* offsetPivot)
    {
        if (btRigidBody)
        {
            return info[(int)JointBody::Target].Position + offsetPivot[(int)JointBody::Target] -
                bodyTarget->GetCenterOfMass();
        }

        return info[(int)JointBody::Target].Position + offsetPivot[(int)JointBody::Target];
    }

    bool BulletJoint::IsJointBroken()
    {
        if (!_btJoint)
            return false;

        btJointFeedback* feedBack = _btJoint->getJointFeedback();

        if (!feedBack)
            return false;

        if (ToVector3(feedBack->m_appliedForceBodyA).Length() > _joint->GetBreakForce())
            return true;
        if (ToVector3(feedBack->m_appliedForceBodyB).Length() > _joint->GetBreakForce())
            return true;

        if (ToVector3(feedBack->m_appliedTorqueBodyA).Length() > _joint->GetBreakTorque())
            return true;
        if (ToVector3(feedBack->m_appliedTorqueBodyB).Length() > _joint->GetBreakTorque())
            return true;

        return false;
    }
}
