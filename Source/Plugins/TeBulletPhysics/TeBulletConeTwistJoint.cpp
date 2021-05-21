#include "TeBulletConeTwistJoint.h"
#include "TeBulletPhysics.h"
#include "TeBulletFJoint.h"

namespace te
{ 
    BulletConeTwistJoint::BulletConeTwistJoint(BulletPhysics* physics, BulletScene* scene)
        : ConeTwistJoint()
        , BulletJoint(physics, scene, this)
    {
        _internal = te_new<BulletFJoint>(physics, scene, this);
    }

    BulletConeTwistJoint::~BulletConeTwistJoint()
    {
        te_delete((BulletFJoint*)_internal);
    }

    void BulletConeTwistJoint::SetBody(JointBody body, RigidBody* value)
    {
        Joint::SetBody(body, value);
        BuildJoint();
    }

    void BulletConeTwistJoint::SetTransform(JointBody body, const Vector3& position, const Quaternion& rotation)
    {
        Joint::SetTransform(body, position, rotation);
        UpdateJoint();
    }

    void BulletConeTwistJoint::SetBreakForce(float force)
    {
        Joint::SetBreakForce(force);
    }

    void BulletConeTwistJoint::SetBreakTorque(float torque)
    {
        Joint::SetBreakTorque(torque);
    }

    void BulletConeTwistJoint::SetEnableCollision(bool collision)
    {
        Joint::SetEnableCollision(collision);
        BuildJoint();
    }

    void BulletConeTwistJoint::SetOffsetPivot(JointBody body, const Vector3& offset)
    {
        Joint::SetOffsetPivot(body, offset);
        BuildJoint();
    }

    void BulletConeTwistJoint::BuildJoint()
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

    void BulletConeTwistJoint::UpdateJoint()
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

    void BulletConeTwistJoint::ReleaseJoint()
    {
        if (!_btJoint)
            return;

        _scene->RemoveJoint(_btJoint);

        te_delete((btConeTwistConstraint*)_btJoint);
        _btJoint = nullptr;
    }
}
