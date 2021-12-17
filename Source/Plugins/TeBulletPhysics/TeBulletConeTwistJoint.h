#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeConeTwistJoint.h"
#include "TeBulletJoint.h"

namespace te
{
    /** Bullet implementation of a ConeTwist joint. */
    class BulletConeTwistJoint : public ConeTwistJoint, public BulletJoint
    {
    public:
        BulletConeTwistJoint(BulletPhysics* physics, BulletScene* scene);
        ~BulletConeTwistJoint();

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

        /** @copydoc ConeTwistJoint::SetDamping */
        void SetDamping(float damping) override;

        /** @copydoc ConeTwistJoint::SetSoftnessLimit */
        void SetSoftnessLimit(float softness) override;

        /** @copydoc ConeTwistJoint::SetBiasLimit */
        void SetBiasLimit(float bias) override;

        /** @copydoc ConeTwistJoint::SetRelaxationLimit */
        void SetRelaxationLimit(float relaxation) override;

        /** @copydoc ConeTwistJoint::SetSwingSpan1 */
        void SetSwingSpan1(Degree deg) override;

        /** @copydoc ConeTwistJoint::SetSwingSpan2 */
        void SetSwingSpan2(Degree deg) override;

        /** @copydoc ConeTwistJoint::SetTwistSpan */
        void SetTwistSpan(Degree deg) override;

        /** @copydoc ConeTwistJoint::SetAngularOnly */
        void SetAngularOnly(bool angularOnly) override;

        /** @copydoc ConeTwistJoint::MotorEnabled */
        void SetMotorEnabled(bool motorEnabled) override;

        /** @copydoc ConeTwistJoint::MotorEnabled */
        void SetMaxMotorImpulse(float motorImpulse) override;

    private:
        /** @copydoc BulletJoint::BuildJoint */
        void BuildJoint() override;

        /** @copydoc BulletJoint::UpdateJoint */
        void UpdateJoint() override;

        /** @copydoc BulletJoint::ReleaseJoint */
        void ReleaseJoint() override;
    };
}
