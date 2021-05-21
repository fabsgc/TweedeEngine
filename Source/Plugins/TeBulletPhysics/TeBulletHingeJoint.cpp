#include "TeBulletHingeJoint.h"
#include "TeBulletPhysics.h"
#include "TeBulletFJoint.h"

namespace te
{
    BulletHingeJoint::BulletHingeJoint(BulletPhysics* physics, BulletScene* scene)
        : HingeJoint()
        , BulletJoint(physics, scene, this)
    {
        _internal = te_new<BulletFJoint>(physics, scene, this);
    }

    BulletHingeJoint::~BulletHingeJoint()
    {
        te_delete((BulletFJoint*)_internal);
    }

    void BulletHingeJoint::SetBody(JointBody body, RigidBody* value)
    {
        Joint::SetBody(body, value);
        BuildJoint();
    }

    void BulletHingeJoint::SetTransform(JointBody body, const Vector3& position, const Quaternion& rotation)
    {
        Joint::SetTransform(body, position, rotation);
        UpdateJoint();
    }

    void BulletHingeJoint::SetBreakForce(float force)
    {
        Joint::SetBreakForce(force);
    }

    void BulletHingeJoint::SetBreakTorque(float torque)
    {
        Joint::SetBreakTorque(torque);
    }

    void BulletHingeJoint::SetEnableCollision(bool collision)
    {
        Joint::SetEnableCollision(collision);
        BuildJoint();
    }

    void BulletHingeJoint::SetOffsetPivot(JointBody body, const Vector3& offset)
    {
        Joint::SetOffsetPivot(body, offset);
        BuildJoint();
    }

    void BulletHingeJoint::BuildJoint()
    {
        ReleaseJoint();

        RigidBody* bodyAnchor = _bodies[(int)JointBody::Anchor].BodyElt;
        RigidBody* bodyTarget = _bodies[(int)JointBody::Target].BodyElt;

        btRigidBody* btBodyAnchor = GetBtRigidBody(&_bodies[(int)JointBody::Anchor]);
        btRigidBody* btBodyTarget = GetBtRigidBody(&_bodies[(int)JointBody::Target]);

        if (btBodyAnchor)
        {

        }
    }

    void BulletHingeJoint::UpdateJoint()
    {
        if (!_btJoint)
        {
            BuildJoint();
            return;
        }

        RigidBody* bodyAnchor = _bodies[(int)JointBody::Anchor].BodyElt;
        RigidBody* bodyTarget = _bodies[(int)JointBody::Target].BodyElt;

        btRigidBody* btBodyTarget = GetBtRigidBody(&_bodies[(int)JointBody::Target]);

        if (bodyAnchor)
        {

        }
    }

    void BulletHingeJoint::ReleaseJoint()
    {
        if (!_btJoint)
            return;

        _scene->RemoveJoint(_btJoint);

        te_delete((btHingeConstraint*)_btJoint);
        _btJoint = nullptr;
    }
}
