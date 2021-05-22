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
        void Update() override { }

        /** @copydoc Joint::SetBody */
        void SetBody(JointBody body, RigidBody* value) override;

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
        void SetIsBroken(bool isBroken);

        /** @copydoc ConeTwistJoint::SetDamping */
        void SetDamping(float damping);

        /** @copydoc ConeTwistJoint::SetLimitSoftness */
        void SetLimitSoftness(float softness);

        /** @copydoc ConeTwistJoint::SetLimitBias */
        void SetLimitBias(float bias);

        /** @copydoc ConeTwistJoint::SetLimitRelaxation */
        void SetLimitRelaxation(float relaxation);

        /** @copydoc ConeTwistJoint::SetSwingSpan1 */
        void SetSwingSpan1(Degree deg);

        /** @copydoc ConeTwistJoint::SetSwingSpan2 */
        void SetSwingSpan2(Degree deg);

        /** @copydoc ConeTwistJoint::SetTwistSpan */
        void SetTwistSpan(Degree deg);

        /** Specify if this joint affect angular properties only */
        void SetAngularOnly(bool angularOnly);

    private:
        /** @copydoc BulletJoint::BuildJoint */
        void BuildJoint() override;

        /** @copydoc BulletJoint::UpdateJoint */
        void UpdateJoint() override;

        /** @copydoc BulletJoint::ReleaseJoint */
        void ReleaseJoint() override;
    };
}
