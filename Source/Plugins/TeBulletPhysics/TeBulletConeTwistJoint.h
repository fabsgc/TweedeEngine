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

    private:
        /** @copydoc BulletJoint::BuildJoint */
        void BuildJoint() override;

        /** @copydoc BulletJoint::UpdateJoint */
        void UpdateJoint() override;

        /** @copydoc BulletJoint::ReleaseJoint */
        void ReleaseJoint() override;
    };
}
