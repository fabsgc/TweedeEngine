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

    void BulletConeTwistJoint::Update()
    {
        if (!_isBroken && IsJointBroken())
        {
            _isBroken = true;
            OnJointBreak();
        }
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

    void BulletConeTwistJoint::SetIsBroken(bool isBroken)
    {
        Joint::SetIsBroken(isBroken);
        BuildJoint();
    }

    void BulletConeTwistJoint::SetDamping(float damping)
    {
        ConeTwistJoint::SetDamping(damping);
        UpdateJoint();
    }

    void BulletConeTwistJoint::SetSoftnessLimit(float softness)
    {
        ConeTwistJoint::SetSoftnessLimit(softness);
        UpdateJoint();
    }

    void BulletConeTwistJoint::SetBiasLimit(float bias)
    {
        ConeTwistJoint::SetBiasLimit(bias);
        UpdateJoint();
    }

    void BulletConeTwistJoint::SetRelaxationLimit(float relaxation)
    {
        ConeTwistJoint::SetRelaxationLimit(relaxation);
        UpdateJoint();
    }

    void BulletConeTwistJoint::SetSwingSpan1(Degree deg)
    {
        ConeTwistJoint::SetSwingSpan1(deg);
        UpdateJoint();
    }

    void BulletConeTwistJoint::SetSwingSpan2(Degree deg)
    {
        ConeTwistJoint::SetSwingSpan2(deg);
        UpdateJoint();
    }

    void BulletConeTwistJoint::SetTwistSpan(Degree deg)
    {
        ConeTwistJoint::SetTwistSpan(deg);
        UpdateJoint();
    }

    void BulletConeTwistJoint::SetAngularOnly(bool angularOnly)
    {
        ConeTwistJoint::SetAngularOnly(angularOnly);
        UpdateJoint();
    }

    void BulletConeTwistJoint::SetMotorEnabled(bool motorEnabled)
    {
        ConeTwistJoint::SetMotorEnabled(motorEnabled);
        UpdateJoint();
    }

    void BulletConeTwistJoint::SetMaxMotorImpulse(float motorImpulse)
    {
        ConeTwistJoint::SetMaxMotorImpulse(motorImpulse);
        UpdateJoint();
    }

    void BulletConeTwistJoint::BuildJoint()
    {
        ReleaseJoint();

        if (_isBroken)
            return;

        RigidBody* bodyAnchor = _bodies[(int)JointBody::Anchor].BodyElt;
        RigidBody* bodyTarget = _bodies[(int)JointBody::Target].BodyElt;

        btRigidBody* btBodyAnchor = static_cast<btRigidBody*>(GetBtCollisionObject(&_bodies[(int)JointBody::Anchor]));
        btRigidBody* btBodyTarget = static_cast<btRigidBody*>(GetBtCollisionObject(&_bodies[(int)JointBody::Target]));

        if (btBodyAnchor)
        {
            Vector3 anchorScaledPosition = GetAnchorScaledPosisition(bodyAnchor, _bodies, _offsetPivots);
            Vector3 targetScaledPosition = GetTargetScaledPosisition(btBodyTarget, bodyTarget, _bodies, _offsetPivots);

            Quaternion anchorRotation = _bodies[(int)JointBody::Anchor].Rotation;
            Quaternion targetRotation = _bodies[(int)JointBody::Target].Rotation;

            btTransform anchorFrame(ToBtQuaternion(anchorRotation), ToBtVector3(anchorScaledPosition));
            btTransform targetframe(ToBtQuaternion(targetRotation), ToBtVector3(targetScaledPosition));

            if (!btBodyTarget)
                btBodyTarget = &btTypedConstraint::getFixedBody();

            btConeTwistConstraint* btConeTwistJoint = te_new<btConeTwistConstraint>(*btBodyAnchor, *btBodyTarget, anchorFrame, targetframe);

            if (btConeTwistJoint)
            {
                _btFeedBack = te_new<btJointFeedback>();

                btConeTwistJoint->setUserConstraintPtr(this);
                btConeTwistJoint->enableFeedback(true);
                btConeTwistJoint->setEnabled(true);
                btConeTwistJoint->setJointFeedback(_btFeedBack);

                btConeTwistJoint->setDamping(_damping);
                btConeTwistJoint->setAngularOnly(_angularOnly);
                btConeTwistJoint->setLimit(_swingSpan1.ValueDegrees(), _swingSpan2.ValueDegrees(), _twistSpan.ValueDegrees(), 
                    _softnessLimit, _biasLimit, _relaxationLimit);
                btConeTwistJoint->enableMotor(_motorEnabled);
                if (_motorEnabled) btConeTwistJoint->setMaxMotorImpulse(_motorImpulse);

                _btJoint = btConeTwistJoint;
                _scene->AddJoint(_btJoint, _enableCollision);
            }
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

        btRigidBody* btBodyTarget = static_cast<btRigidBody*>(GetBtCollisionObject(&_bodies[(int)JointBody::Target]));

        if (bodyAnchor)
        {
            Vector3 anchorScaledPosition = GetAnchorScaledPosisition(bodyAnchor, _bodies, _offsetPivots);
            Vector3 targetScaledPosition = GetTargetScaledPosisition(btBodyTarget, bodyTarget, _bodies, _offsetPivots);

            Quaternion anchorRotation = _bodies[(int)JointBody::Anchor].Rotation;
            Quaternion targetRotation = _bodies[(int)JointBody::Target].Rotation;

            btTransform anchorFrame(ToBtQuaternion(anchorRotation), ToBtVector3(anchorScaledPosition));
            btTransform targetframe(ToBtQuaternion(targetRotation), ToBtVector3(targetScaledPosition));

            btConeTwistConstraint* btConeTwistJoint = (btConeTwistConstraint*)_btJoint;

            btConeTwistJoint->setFrames(anchorFrame, targetframe);
            btConeTwistJoint->setDamping(_damping);
            btConeTwistJoint->setAngularOnly(_angularOnly);
            btConeTwistJoint->setLimit(_swingSpan1.ValueRadians(), _swingSpan2.ValueRadians(), _twistSpan.ValueRadians(),
                _softnessLimit, _biasLimit, _relaxationLimit);
            btConeTwistJoint->enableMotor(_motorEnabled);
            if (_motorEnabled) btConeTwistJoint->setMaxMotorImpulse(_motorImpulse);
        }
    }

    void BulletConeTwistJoint::ReleaseJoint()
    {
        if (!_btJoint)
            return;

        _scene->RemoveJoint(_btJoint);

        te_delete((btConeTwistConstraint*)_btJoint);
        te_delete(_btFeedBack);

        _btJoint = nullptr;
        _btFeedBack = nullptr;
    }
}
