#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeD6Joint.h"
#include "TeBulletJoint.h"

namespace te
{
    /** Bullet implementation of a D6 joint. */
    class BulletD6Joint : public D6Joint, public BulletJoint
    {
    public:
        BulletD6Joint(BulletPhysics* physics, BulletScene* scene);
        ~BulletD6Joint();

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

        /** @copydoc SliderJoint::SetLowerLinLimit */
        void SetLowerLinLimit(float lowerLinLimit) override;

        /** @copydoc SliderJoint::SetUpperLinLimit */
        void SetUpperLinLimit(float upperLinLimit) override;

        /** @copydoc D6Joint::SetLowerAngLimit */
        void SetLowerAngLimit(Degree lowerAngLimit) override;

        /** @copydoc D6Joint::SetUpperAngLimit */
        void SetUpperAngLimit(Degree upperAngLimit) override;

        /** @copydoc D6Joint::SetLinearSpring */
        void SetLinearSpring(bool linearSpring) override;

        /** @copydoc D6Joint::SetAngularSpring */
        void SetAngularSpring(bool angularSpring) override;

        /** @copydoc D6Joint::SetLinearStiffness */
        void SetLinearStiffness(float linearStiffness) override;

        /** @copydoc D6Joint::SetAngularStiffness */
        void SetAngularStiffness(float angularStiffness) override;

        /** @copydoc D6Joint::SetLinearDamping */
        void SetLinearDamping(float linearDamping) override;

        /** @copydoc D6Joint::SetAngularDamping */
        void SetAngularDamping(float angularDamping) override;

    private:
        /** @copydoc BulletJoint::BuildJoint */
        void BuildJoint() override;

        /** @copydoc BulletJoint::UpdateJoint */
        void UpdateJoint() override;

        /** @copydoc BulletJoint::ReleaseJoint */
        void ReleaseJoint() override;

        /** @copydoc SliderJoint::SetLinearStiffness */
        void SetStiffness();

        /** @copydoc SliderJoint::SetLinearDamping */
        void SetDamping();
    };
}
