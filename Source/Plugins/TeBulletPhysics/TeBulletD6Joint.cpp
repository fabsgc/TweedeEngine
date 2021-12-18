#include "TeBulletD6Joint.h"
#include "TeBulletPhysics.h"
#include "TeBulletFJoint.h"

namespace te
{
    BulletD6Joint::BulletD6Joint(BulletPhysics* physics, BulletScene* scene)
        : D6Joint()
        , BulletJoint(physics, scene, this)
    {
        _internal = te_new<BulletFJoint>(physics, scene, this);
    }

    BulletD6Joint::~BulletD6Joint()
    {
        ReleaseJoint();
        te_delete((BulletFJoint*)_internal);
    }

    void BulletD6Joint::Update()
    {
        if (!_isBroken && IsJointBroken())
        {
            _isBroken = true;
            OnJointBreak();
        }
    }

    void BulletD6Joint::SetBody(JointBody body, RigidBody* value)
    {
        Joint::SetBody(body, value);
        BuildJoint();
    }

    void BulletD6Joint::SetTransform(JointBody body, const Vector3& position, const Quaternion& rotation)
    {
        Joint::SetTransform(body, position, rotation);
        UpdateJoint();
    }

    void BulletD6Joint::SetBreakForce(float force)
    {
        Joint::SetBreakForce(force);
    }

    void BulletD6Joint::SetBreakTorque(float torque)
    {
        Joint::SetBreakTorque(torque);
    }

    void BulletD6Joint::SetEnableCollision(bool collision)
    {
        Joint::SetEnableCollision(collision);
        BuildJoint();
    }

    void BulletD6Joint::SetOffsetPivot(JointBody body, const Vector3& offset)
    {
        Joint::SetOffsetPivot(body, offset);
        BuildJoint();
    }

    void BulletD6Joint::SetIsBroken(bool isBroken)
    {
        Joint::SetIsBroken(isBroken);
        BuildJoint();
    }

    void BulletD6Joint::SetLowerLinLimit(float lowerLinLimit)
    {
        D6Joint::SetLowerLinLimit(lowerLinLimit);
        UpdateJoint();
    }

    void BulletD6Joint::SetUpperLinLimit(float upperLinLimit)
    {
        D6Joint::SetUpperLinLimit(upperLinLimit);
        UpdateJoint();
    }

    void BulletD6Joint::SetLowerAngLimit(Degree lowerAngLimit)
    {
        D6Joint::SetLowerAngLimit(lowerAngLimit);
        UpdateJoint();
    }

    void BulletD6Joint::SetUpperAngLimit(Degree upperAngLimit)
    {
        D6Joint::SetUpperAngLimit(upperAngLimit);
        UpdateJoint();
    }

    void BulletD6Joint::SetLinearSpring(bool linearSpring)
    {
        D6Joint::SetLinearSpring(linearSpring);
        UpdateJoint();
    }

    void BulletD6Joint::SetAngularSpring(bool angularSpring)
    {
        D6Joint::SetAngularSpring(angularSpring);
        UpdateJoint();
    }

    void BulletD6Joint::SetLinearStiffness(float linearStiffness)
    {
        D6Joint::SetLinearStiffness(linearStiffness);
        UpdateJoint();
    }

    void BulletD6Joint::SetAngularStiffness(float angularStiffness)
    {
        D6Joint::SetAngularStiffness(angularStiffness);
        UpdateJoint();
    }

    void BulletD6Joint::SetLinearDamping(float linearDamping)
    {
        D6Joint::SetLinearDamping(linearDamping);
        UpdateJoint();
    }

    void BulletD6Joint::SetAngularDamping(float angularDamping)
    {
        D6Joint::SetAngularDamping(angularDamping);
        UpdateJoint();
    }

    void BulletD6Joint::BuildJoint()
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

            btGeneric6DofSpringConstraint* btD6Joint = te_new<btGeneric6DofSpringConstraint>(*btBodyAnchor, *btBodyTarget, anchorFrame, targetframe, false);

            if (btD6Joint)
            {
                _btFeedBack = te_new<btJointFeedback>();

                btD6Joint->setUserConstraintPtr(this);
                btD6Joint->enableFeedback(true);
                btD6Joint->setEnabled(true);
                btD6Joint->setJointFeedback(_btFeedBack);

                btD6Joint->setLinearLowerLimit(btVector3(_lowerLinLimit, _lowerLinLimit, _lowerLinLimit));
                btD6Joint->setLinearUpperLimit(btVector3(_upperLinLimit, _upperLinLimit, _upperLinLimit));
                btD6Joint->setAngularLowerLimit(btVector3(_lowerAngLimit.ValueRadians(), _lowerAngLimit.ValueRadians(), _lowerAngLimit.ValueRadians()));
                btD6Joint->setAngularUpperLimit(btVector3(_upperAngLimit.ValueRadians(), _upperAngLimit.ValueRadians(), _upperAngLimit.ValueRadians()));

                SetStiffness();
                SetDamping();

                _btJoint = btD6Joint;
                _scene->AddJoint(_btJoint, _enableCollision);
            }
        }
    }

    void BulletD6Joint::UpdateJoint()
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

            btGeneric6DofSpringConstraint* btD6Joint = (btGeneric6DofSpringConstraint*)_btJoint;

            btD6Joint->setFrames(anchorFrame, targetframe);

            btD6Joint->setLinearLowerLimit(btVector3(_lowerLinLimit, _lowerLinLimit, _lowerLinLimit));
            btD6Joint->setLinearUpperLimit(btVector3(_upperLinLimit, _upperLinLimit, _upperLinLimit));
            btD6Joint->setAngularLowerLimit(btVector3(_lowerAngLimit.ValueRadians(), _lowerAngLimit.ValueRadians(), _lowerAngLimit.ValueRadians()));
            btD6Joint->setAngularUpperLimit(btVector3(_upperAngLimit.ValueRadians(), _upperAngLimit.ValueRadians(), _upperAngLimit.ValueRadians()));

            SetStiffness();
            SetDamping();
        }
    }

    void BulletD6Joint::ReleaseJoint()
    {
        if (!_btJoint)
            return;

        _scene->RemoveJoint(_btJoint);

        te_delete((btGeneric6DofSpringConstraint*)_btJoint);
        te_delete(_btFeedBack);

        _btJoint = nullptr;
        _btFeedBack = nullptr;
    }

    void BulletD6Joint::SetStiffness()
    {
        btGeneric6DofSpring2Constraint* btD6Joint = (btGeneric6DofSpring2Constraint*)_btJoint;

        btD6Joint->setStiffness(0, _linearStiffness);
        btD6Joint->setStiffness(1, _linearStiffness);
        btD6Joint->setStiffness(2, _linearStiffness);

        btD6Joint->setStiffness(3, _angularStiffness);
        btD6Joint->setStiffness(4, _angularStiffness);
        btD6Joint->setStiffness(5, _angularStiffness);
    }

    void BulletD6Joint::SetDamping()
    {
        btGeneric6DofSpring2Constraint* btD6Joint = (btGeneric6DofSpring2Constraint*)_btJoint;

        btD6Joint->setDamping(0, _linearDamping);
        btD6Joint->setDamping(1, _linearDamping);
        btD6Joint->setDamping(2, _linearDamping);

        btD6Joint->setDamping(3, _angularDamping);
        btD6Joint->setDamping(4, _angularDamping);
        btD6Joint->setDamping(5, _angularDamping);
    }
}
