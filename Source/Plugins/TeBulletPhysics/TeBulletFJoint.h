#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeFJoint.h"

namespace te
{
    /** Bullet implementation of an FJoint. */
    class BulletFJoint : public FJoint
    {
    public:
        BulletFJoint(BulletPhysics* physics, BulletScene* scene, BulletJoint* parent, const JOINT_DESC& desc);
        ~BulletFJoint();

        /** @copydoc FJoint::GetBody */
        Body* GetBody(JointBody body) const override;

        /** @copydoc FJoint::SetBody */
        void SetBody(JointBody body, Body* value) override;

        /** @copydoc FJoint::GetPosition */
        Vector3 GetPosition(JointBody body) const override;

        /** @copydoc FJoint::GetRotation */
        Quaternion GetRotation(JointBody body) const override;

        /** @copydoc FJoint::SetTransform */
        void SetTransform(JointBody body, const Vector3& position, const Quaternion& rotation) override;

        /** @copydoc FJoint::GetBreakForce */
        float GetBreakForce() const override;

        /** @copydoc FJoint::SetBreakForce */
        void SetBreakForce(float force) override;

        /** @copydoc FJoint::GetBreakTorque */
        float GetBreakTorque() const override;

        /** @copydoc FJoint::SetBreakTorque */
        void SetBreakTorque(float torque) override;

        /** @copydoc FJoint::GetEnableCollision */
        bool GetEnableCollision() const override;

        /** @copydoc FJoint::SetEnableCollision */
        void SetEnableCollision(bool value) override;

    protected:
        friend class BulletJoint;

        BulletPhysics* _physics;
        BulletScene* _scene;

        BulletJoint* _parent;
    };
}
