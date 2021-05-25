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

    void BulletHingeJoint::Update()
    {
        if (!_isBroken && IsJointBroken())
        {
            _isBroken = true;
            OnJointBreak();
        }
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

    void BulletHingeJoint::SetIsBroken(bool isBroken)
    {
        Joint::SetIsBroken(isBroken);
        BuildJoint();
    }

    void BulletHingeJoint::SetLimitSoftness(float softness)
    {
        HingeJoint::SetLimitSoftness(softness);
        UpdateJoint();
    }

    void BulletHingeJoint::SetLimitBias(float bias)
    {
        HingeJoint::SetLimitBias(bias);
        UpdateJoint();
    }

    void BulletHingeJoint::SetLimitRelaxation(float relaxation)
    {
        HingeJoint::SetLimitRelaxation(relaxation);
        UpdateJoint();
    }

    void BulletHingeJoint::SetLimitHigh(Degree limitHigh)
    {
        HingeJoint::SetLimitHigh(limitHigh);
        UpdateJoint();
    }

    void BulletHingeJoint::SetLimitLow(Degree limitLow)
    {
        HingeJoint::SetLimitLow(limitLow);
        UpdateJoint();
    }

    void BulletHingeJoint::SetAngularOnly(bool angularOnly)
    {
        HingeJoint::SetAngularOnly(angularOnly);
        UpdateJoint();
    }

    void BulletHingeJoint::SetMotorEnabled(bool motorEnabled)
    {
        HingeJoint::SetMotorEnabled(motorEnabled);
        UpdateJoint();
    }

    void BulletHingeJoint::SetMaxMotorImpulse(float motorImpulse)
    {
        HingeJoint::SetMaxMotorImpulse(motorImpulse);
        UpdateJoint();
    }

    void BulletHingeJoint::SetMotorVelocity(float motorVelocity)
    {
        HingeJoint::SetMotorVelocity(motorVelocity);
        UpdateJoint();
    }

    void BulletHingeJoint::BuildJoint()
    {
        ReleaseJoint();

        if (_isBroken)
            return;

        RigidBody* bodyAnchor = _bodies[(int)JointBody::Anchor].BodyElt;
        RigidBody* bodyTarget = _bodies[(int)JointBody::Target].BodyElt;

        btRigidBody* btBodyAnchor = GetBtRigidBody(&_bodies[(int)JointBody::Anchor]);
        btRigidBody* btBodyTarget = GetBtRigidBody(&_bodies[(int)JointBody::Target]);

        if (btBodyAnchor)
        {
            Vector3 anchorScaledPosition = GetAnchorScaledPosisition(bodyAnchor, _bodies, _offsetPivots);
            Vector3 targetScaledPosition = GetTargetScaledPosisition(btBodyTarget, bodyTarget, _bodies, _offsetPivots);

            if (!btBodyTarget)
                btBodyTarget = &btTypedConstraint::getFixedBody();

            Quaternion anchorRotation = _bodies[(int)JointBody::Anchor].Rotation;
            Quaternion targetRotation = _bodies[(int)JointBody::Target].Rotation;

            btTransform anchorFrame(ToBtQuaternion(anchorRotation), ToBtVector3(anchorScaledPosition));
            btTransform targetframe(ToBtQuaternion(targetRotation), ToBtVector3(targetScaledPosition));

            if (!btBodyTarget)
                btBodyTarget = &btTypedConstraint::getFixedBody();

            btHingeConstraint* btHingeJoint = te_new<btHingeConstraint>(*btBodyAnchor, *btBodyTarget, anchorFrame, targetframe);

            if (btHingeJoint)
            {
                _btFeedBack = te_new<btJointFeedback>();

                btHingeJoint->setUserConstraintPtr(this);
                btHingeJoint->enableFeedback(true);
                btHingeJoint->setEnabled(true);
                btHingeJoint->setJointFeedback(_btFeedBack);

                btHingeJoint->setAngularOnly(_angularOnly);
                btHingeJoint->setLimit(_limitLow.ValueRadians(), _limitHigh.ValueRadians(), _limitSoftness, _limitBias, _limitRelaxation);
                btHingeJoint->enableMotor(_motorEnabled);

                if (_motorEnabled)
                {
                    btHingeJoint->setMaxMotorImpulse(_motorImpulse);
                    btHingeJoint->setMotorTargetVelocity(_motorVelocity);
                }

                _btJoint = btHingeJoint;
                _scene->AddJoint(_btJoint, _enableCollision);
            }
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
            Vector3 anchorScaledPosition = GetAnchorScaledPosisition(bodyAnchor, _bodies, _offsetPivots);
            Vector3 targetScaledPosition = GetTargetScaledPosisition(btBodyTarget, bodyTarget, _bodies, _offsetPivots);

            Quaternion anchorRotation = _bodies[(int)JointBody::Anchor].Rotation;
            Quaternion targetRotation = _bodies[(int)JointBody::Target].Rotation;

            btTransform anchorFrame(ToBtQuaternion(anchorRotation), ToBtVector3(anchorScaledPosition));
            btTransform targetframe(ToBtQuaternion(targetRotation), ToBtVector3(targetScaledPosition));

            btHingeConstraint* btHingeJoint = (btHingeConstraint*)_btJoint;

            btHingeJoint->setFrames(anchorFrame, targetframe);
            btHingeJoint->setAngularOnly(_angularOnly);
            btHingeJoint->setLimit(_limitLow.ValueRadians(), _limitHigh.ValueRadians(), _limitSoftness, _limitBias, _limitRelaxation);
            btHingeJoint->enableMotor(_motorEnabled);

            if (_motorEnabled)
            {
                btHingeJoint->setMaxMotorImpulse(_motorImpulse);
                btHingeJoint->setMotorTargetVelocity(_motorVelocity);
            }
        }
    }

    void BulletHingeJoint::ReleaseJoint()
    {
        if (!_btJoint)
            return;

        _scene->RemoveJoint(_btJoint);

        te_delete((btHingeConstraint*)_btJoint);
        te_delete(_btFeedBack);

        _btJoint = nullptr;
        _btFeedBack = nullptr;
    }
}
