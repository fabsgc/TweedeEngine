#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeHingeJoint.h"
#include "TeBulletJoint.h"

namespace te
{
    /** Bullet implementation of a Hinge joint. */
    class BulletHingeJoint : public HingeJoint, public BulletJoint
    {
    public:
        BulletHingeJoint(BulletPhysics* physics, BulletScene* scene);
        ~BulletHingeJoint();

        /** @copydoc Joint::Update */
        void Update() override;

        /** @copydoc Joint::SetBody */
        void SetBody(JointBody body, Body* value) override;

        /** @copydoc Joint::SetTransform  */
        void SetTransform(JointBody body, const Vector3& position, const Quaternion& rotation) override;

        /** @copydoc Joint::SetBreakForce */
        void SetBreakForce(float force) override;

        /** @copydoc Joint::SetBreakTorque */
        void SetBreakTorque(float torque) override;

        /** @copydoc Joint::SetEnableCollision */
        void SetEnableCollision(bool collision) override;

        /** @copydoc Joint::SetOffsetPivot */
        void SetOffsetPivot(JointBody body, const Vector3& offset) override;

        /** @copydoc Joint::IsBroken */
        void SetIsBroken(bool isBroken) override;

        /** @copydoc HingeJoint::SetSoftnessLimit */
        void SetSoftnessLimit(float softness) override;

        /** @copydoc HingeJoint::SetBiasLimit */
        void SetBiasLimit(float bias) override;

        /** @copydoc HingeJoint::SetRelaxationLimit */
        void SetRelaxationLimit(float relaxation) override;

        /** @copydoc HingeJoint::SetHighLimit */
        void SetHighLimit(Degree highLimit) override;

        /** @copydoc HingeJoint::SetLowLimit */
        void SetLowLimit(Degree lowLimit) override;

        /** @copydoc HingeJoint::SetAngularOnly */
        void SetAngularOnly(bool angularOnly) override;

        /** @copydoc HingeJoint::SetMotorEnabled */
        void SetMotorEnabled(bool motorEnabled) override;

        /** @copydoc HingeJoint::SetMaxMotorImpulse */
        void SetMaxMotorImpulse(float motorImpulse) override;

        /** @copydoc HingeJoint::SetMotorVelocity */
        void SetMotorVelocity(float motorVelocity) override;

    private:
        /** @copydoc BulletJoint::BuildJoint */
        void BuildJoint() override;

        /** @copydoc BulletJoint::UpdateJoint */
        void UpdateJoint() override;

        /** @copydoc BulletJoint::ReleaseJoint */
        void ReleaseJoint() override;
    };
}
