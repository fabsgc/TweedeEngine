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

    void BulletSliderJoint::BuildJoint()
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

            btSliderConstraint* btSliderJoint = te_new<btSliderConstraint>(*btBodyAnchor, *btBodyTarget, anchorFrame, targetframe, false);

            if (btSliderJoint)
            {
                _btFeedBack = te_new<btJointFeedback>();

                btSliderJoint->setUserConstraintPtr(this);
                btSliderJoint->enableFeedback(true);
                btSliderJoint->setEnabled(true);
                btSliderJoint->setJointFeedback(_btFeedBack);

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

        btRigidBody* btBodyTarget = GetBtRigidBody(&_bodies[(int)JointBody::Target]);

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
