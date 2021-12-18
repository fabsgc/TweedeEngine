#include "TeBulletSliderJoint.h"
#include "TeBulletPhysics.h"
#include "TeBulletFJoint.h"

namespace te
{
    BulletSliderJoint::BulletSliderJoint(BulletPhysics* physics, BulletScene* scene)
        : SliderJoint()
        , BulletJoint(physics, scene, this)
    {
        _internal = te_new<BulletFJoint>(physics, scene, this);
    }

    BulletSliderJoint::~BulletSliderJoint()
    {
        ReleaseJoint();
        te_delete((BulletFJoint*)_internal);
    }

    void BulletSliderJoint::Update()
    {
        if (!_isBroken && IsJointBroken())
        {
            _isBroken = true;
            OnJointBreak();
        }
    }

    void BulletSliderJoint::SetBody(JointBody body, RigidBody* value)
    {
        Joint::SetBody(body, value);
        BuildJoint();
    }

    void BulletSliderJoint::SetTransform(JointBody body, const Vector3& position, const Quaternion& rotation)
    {
        Joint::SetTransform(body, position, rotation);
        UpdateJoint();
    }

    void BulletSliderJoint::SetBreakForce(float force)
    {
        Joint::SetBreakForce(force);
    }

    void BulletSliderJoint::SetBreakTorque(float torque)
    {
        Joint::SetBreakTorque(torque);
    }

    void BulletSliderJoint::SetEnableCollision(bool collision)
    {
        Joint::SetEnableCollision(collision);
        BuildJoint();
    }

    void BulletSliderJoint::SetOffsetPivot(JointBody body, const Vector3& offset)
    {
        Joint::SetOffsetPivot(body, offset);
        BuildJoint();
    }

    void BulletSliderJoint::SetIsBroken(bool isBroken)
    {
        Joint::SetIsBroken(isBroken);
        BuildJoint();
    }

    void BulletSliderJoint::SetLowerLinLimit(float lowerLinLimit)
    {
        SliderJoint::SetLowerLinLimit(lowerLinLimit);
        UpdateJoint();
    }

    void BulletSliderJoint::SetUpperLinLimit(float upperLinLimit)
    {
        SliderJoint::SetUpperLinLimit(upperLinLimit);
        UpdateJoint();
    }

    void BulletSliderJoint::SetLowerAngLimit(Degree lowerAngLimit)
    {
        SliderJoint::SetLowerAngLimit(lowerAngLimit);
        UpdateJoint();
    }

    void BulletSliderJoint::SetUpperAngLimit(Degree upperAngLimit)
    {
        SliderJoint::SetUpperAngLimit(upperAngLimit);
        UpdateJoint();
    }

    void BulletSliderJoint::SetSoftnessDirLin(float softnessDirLin)
    {
        SliderJoint::SetSoftnessDirLin(softnessDirLin);
        UpdateJoint();
    }

    void BulletSliderJoint::SetRestitutionDirLin(float restitutionDirLin)
    {
        SliderJoint::SetRestitutionDirLin(restitutionDirLin);
        UpdateJoint();
    }

    void BulletSliderJoint::SetDampingDirLin(float dampingDirLin)
    {
        SliderJoint::SetDampingDirLin(dampingDirLin);
        UpdateJoint();
    }

    void BulletSliderJoint::SetSoftnessDirAng(float softnessDirAng)
    {
        SliderJoint::SetSoftnessDirAng(softnessDirAng);
        UpdateJoint();
    }

    void BulletSliderJoint::SetRestitutionDirAng(float restitutionDirAng)
    {
        SliderJoint::SetRestitutionDirAng(restitutionDirAng);
        UpdateJoint();
    }

    void BulletSliderJoint::SetDampingDirAng(float dampingDirAng)
    {
        SliderJoint::SetDampingDirAng(dampingDirAng);
        UpdateJoint();
    }

    void BulletSliderJoint::SetSoftnessLimLin(float softnessLimLin)
    {
        SliderJoint::SetSoftnessLimLin(softnessLimLin);
        UpdateJoint();
    }

    void BulletSliderJoint::SetRestitutionLimLin(float restitutionLimLin)
    {
        SliderJoint::SetRestitutionLimLin(restitutionLimLin);
        UpdateJoint();
    }

    void BulletSliderJoint::SetDampingLimLin(float dampingLimLin)
    {
        SliderJoint::SetDampingLimLin(dampingLimLin);
        UpdateJoint();
    }

    void BulletSliderJoint::SetSoftnessLimAng(float softnessLimAng)
    {
        SliderJoint::SetSoftnessLimAng(softnessLimAng);
        UpdateJoint();
    }

    void BulletSliderJoint::SetRestitutionLimAng(float restitutionLimAng)
    {
        SliderJoint::SetRestitutionLimAng(restitutionLimAng);
        UpdateJoint();
    }

    void BulletSliderJoint::SetDampingLimAng(float dampingLimAng)
    {
        SliderJoint::SetDampingLimAng(dampingLimAng);
        UpdateJoint();
    }

    void BulletSliderJoint::SetSoftnessOrthoLin(float softnessOrthoLin)
    {
        SliderJoint::SetSoftnessOrthoLin(softnessOrthoLin);
        UpdateJoint();
    }

    void BulletSliderJoint::SetRestitutionOrthoLin(float restitutionOrthoLin)
    {
        SliderJoint::SetRestitutionOrthoLin(restitutionOrthoLin);
        UpdateJoint();
    }

    void BulletSliderJoint::SetDampingOrthoLin(float dampingOrthoLin)
    {
        SliderJoint::SetDampingOrthoLin(dampingOrthoLin);
        UpdateJoint();
    }

    void BulletSliderJoint::SetSoftnessOrthoAng(float softnessOrthoAng)
    {
        SliderJoint::SetSoftnessOrthoAng(softnessOrthoAng);
        UpdateJoint();
    }

    void BulletSliderJoint::SetRestitutionOrthoAng(float restitutionOrthoAng)
    {
        SliderJoint::SetRestitutionOrthoAng(restitutionOrthoAng);
        UpdateJoint();
    }

    void BulletSliderJoint::SetDampingOrthoAng(float dampingOrthoAng)
    {
        SliderJoint::SetDampingOrthoAng(dampingOrthoAng);
        UpdateJoint();
    }

    void BulletSliderJoint::SetPoweredLinMotor(bool poweredLinMotor)
    {
        SliderJoint::SetPoweredLinMotor(poweredLinMotor);
        UpdateJoint();
    }

    void BulletSliderJoint::SetTargetLinMotorVelocity(float targetLinMotorVelocity)
    {
        SliderJoint::SetTargetLinMotorVelocity(targetLinMotorVelocity);
        UpdateJoint();
    }

    void BulletSliderJoint::SetMaxLinMotorForce(float maxLinMotorForce)
    {
        SliderJoint::SetMaxLinMotorForce(maxLinMotorForce);
        UpdateJoint();
    }

    void BulletSliderJoint::SetPoweredAngMotor(bool poweredAngMotor)
    {
        SliderJoint::SetPoweredAngMotor(poweredAngMotor);
        UpdateJoint();
    }

    void BulletSliderJoint::SetTargetAngMotorVelocity(float targetAngMotorVelocity)
    {
        SliderJoint::SetTargetAngMotorVelocity(targetAngMotorVelocity);
        UpdateJoint();
    }

    void BulletSliderJoint::SetMaxAngMotorForce(float maxAngMotorForce)
    {
        SliderJoint::SetMaxAngMotorForce(maxAngMotorForce);
        UpdateJoint();
    }

    void BulletSliderJoint::BuildJoint()
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

            if (!btBodyTarget)
                btBodyTarget = &btTypedConstraint::getFixedBody();

            Quaternion anchorRotation = _bodies[(int)JointBody::Anchor].Rotation;
            Quaternion targetRotation = _bodies[(int)JointBody::Target].Rotation;

            btTransform anchorFrame(ToBtQuaternion(anchorRotation), ToBtVector3(anchorScaledPosition));
            btTransform targetframe(ToBtQuaternion(targetRotation), ToBtVector3(targetScaledPosition));

            if (!btBodyTarget)
                btBodyTarget = &btTypedConstraint::getFixedBody();

            btSliderConstraint* btSliderJoint = te_new<btSliderConstraint>(*btBodyAnchor, *btBodyTarget, anchorFrame, targetframe, false);

            if (btSliderJoint)
            {
                _btFeedBack = te_new<btJointFeedback>();

                btSliderJoint->setUserConstraintPtr(this);
                btSliderJoint->enableFeedback(true);
                btSliderJoint->setEnabled(true);
                btSliderJoint->setJointFeedback(_btFeedBack);

                btSliderJoint->setLowerLinLimit(_lowerLinLimit);
                btSliderJoint->setUpperLinLimit(_upperLinLimit);
                btSliderJoint->setLowerAngLimit(_lowerAngLimit.ValueRadians());
                btSliderJoint->setUpperAngLimit(_upperAngLimit.ValueRadians());

                btSliderJoint->setSoftnessDirLin(_softnessDirLin);
                btSliderJoint->setRestitutionDirLin(_restitutionDirLin);
                btSliderJoint->setDampingDirLin(_dampingDirLin);
                btSliderJoint->setSoftnessDirAng(_softnessDirAng);
                btSliderJoint->setRestitutionDirAng(_restitutionDirAng);
                btSliderJoint->setDampingDirAng(_dampingDirAng);
                btSliderJoint->setSoftnessLimLin(_softnessLimLin);
                btSliderJoint->setRestitutionLimLin(_restitutionLimLin);
                btSliderJoint->setDampingLimLin(_dampingLimLin);
                btSliderJoint->setSoftnessLimAng(_softnessLimAng);
                btSliderJoint->setRestitutionLimAng(_restitutionLimAng);
                btSliderJoint->setDampingLimAng(_dampingLimAng);
                btSliderJoint->setSoftnessOrthoLin(_softnessOrthoLin);
                btSliderJoint->setRestitutionOrthoLin(_restitutionOrthoLin);
                btSliderJoint->setDampingOrthoLin(_dampingOrthoLin);
                btSliderJoint->setSoftnessOrthoAng(_softnessOrthoAng);
                btSliderJoint->setRestitutionOrthoAng(_restitutionOrthoAng);
                btSliderJoint->setDampingOrthoAng(_dampingOrthoAng);
                btSliderJoint->setPoweredLinMotor(_poweredLinMotor);
                btSliderJoint->setTargetLinMotorVelocity(_targetLinMotorVelocity);
                btSliderJoint->setMaxLinMotorForce(_maxLinMotorForce);
                btSliderJoint->setPoweredAngMotor(_poweredAngMotor);
                btSliderJoint->setTargetAngMotorVelocity(_targetAngMotorVelocity);
                btSliderJoint->setMaxAngMotorForce(_maxAngMotorForce);

                _btJoint = btSliderJoint;
                _scene->AddJoint(_btJoint, _enableCollision);
            }
        }
    }

    void BulletSliderJoint::UpdateJoint()
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

            btSliderConstraint* btSliderJoint = (btSliderConstraint*)_btJoint;

            btSliderJoint->setFrames(anchorFrame, targetframe);

            btSliderJoint->setLowerLinLimit(_lowerLinLimit);
            btSliderJoint->setUpperLinLimit(_upperLinLimit);
            btSliderJoint->setLowerAngLimit(_lowerAngLimit.ValueRadians());
            btSliderJoint->setUpperAngLimit(_upperAngLimit.ValueRadians());

            btSliderJoint->setSoftnessDirLin(_softnessDirLin);
            btSliderJoint->setRestitutionDirLin(_restitutionDirLin);
            btSliderJoint->setDampingDirLin(_dampingDirLin);
            btSliderJoint->setSoftnessDirAng(_softnessDirAng);
            btSliderJoint->setRestitutionDirAng(_restitutionDirAng);
            btSliderJoint->setDampingDirAng(_dampingDirAng);
            btSliderJoint->setSoftnessLimLin(_softnessLimLin);
            btSliderJoint->setRestitutionLimLin(_restitutionLimLin);
            btSliderJoint->setDampingLimLin(_dampingLimLin);
            btSliderJoint->setSoftnessLimAng(_softnessLimAng);
            btSliderJoint->setRestitutionLimAng(_restitutionLimAng);
            btSliderJoint->setDampingLimAng(_dampingLimAng);
            btSliderJoint->setSoftnessOrthoLin(_softnessOrthoLin);
            btSliderJoint->setRestitutionOrthoLin(_restitutionOrthoLin);
            btSliderJoint->setDampingOrthoLin(_dampingOrthoLin);
            btSliderJoint->setSoftnessOrthoAng(_softnessOrthoAng);
            btSliderJoint->setRestitutionOrthoAng(_restitutionOrthoAng);
            btSliderJoint->setDampingOrthoAng(_dampingOrthoAng);
            btSliderJoint->setPoweredLinMotor(_poweredLinMotor);
            btSliderJoint->setTargetLinMotorVelocity(_targetLinMotorVelocity);
            btSliderJoint->setMaxLinMotorForce(_maxLinMotorForce);
            btSliderJoint->setPoweredAngMotor(_poweredAngMotor);
            btSliderJoint->setTargetAngMotorVelocity(_targetAngMotorVelocity);
            btSliderJoint->setMaxAngMotorForce(_maxAngMotorForce);
        }
    }

    void BulletSliderJoint::ReleaseJoint()
    {
        if (!_btJoint)
            return;

        _scene->RemoveJoint(_btJoint);

        te_delete((btSliderConstraint*)_btJoint);
        te_delete(_btFeedBack);

        _btJoint = nullptr;
        _btFeedBack = nullptr;
    }
}
