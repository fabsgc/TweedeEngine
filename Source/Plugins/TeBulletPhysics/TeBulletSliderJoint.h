#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeSliderJoint.h"
#include "TeBulletJoint.h"

namespace te
{
    /** Bullet implementation of a Slider joint. */
    class BulletSliderJoint : public SliderJoint, public BulletJoint
    {
    public:
        BulletSliderJoint(BulletPhysics* physics, BulletScene* scene);
        ~BulletSliderJoint();

        /** @copydoc Joint::Update */
        void Update() override;

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
        void SetIsBroken(bool isBroken) override;

        /** @copydoc SliderJoint::SetLowerLinLimit*/
        void SetLowerLinLimit(float lowerLinLimit) override;

        /** @copydoc SliderJoint::SetUpperLinLimit*/
        void SetUpperLinLimit(float upperLinLimit) override;

        /** @copydoc SliderJoint::SetLowerAngLimit*/
        void SetLowerAngLimit(Degree lowerAngLimit) override;

        /** @copydoc SliderJoint::SetUpperAngLimit*/
        void SetUpperAngLimit(Degree upperAngLimit) override;

        /** @copydoc SliderJoint::SetSoftnessDirLin*/
        void SetSoftnessDirLin(float softnessDirLin) override;

        /** @copydoc SliderJoint::SetRestitutionDirLin*/
        void SetRestitutionDirLin(float restitutionDirLin) override;

        /** @copydoc SliderJoint::SetDampingDirLin*/
        void SetDampingDirLin(float dampingDirLin) override;

        /** @copydoc SliderJoint::SetSoftnessDirAng*/
        void SetSoftnessDirAng(float softnessDirAng) override;

        /** @copydoc SliderJoint::SetRestitutionDirAng*/
        void SetRestitutionDirAng(float restitutionDirAng) override;

        /** @copydoc SliderJoint::SetDampingDirAng*/
        void SetDampingDirAng(float dampingDirAng) override;

        /** @copydoc SliderJoint::SetSoftnessLimLin*/
        void SetSoftnessLimLin(float softnessLimLin) override;

        /** @copydoc SliderJoint::SetRestitutionLimLin*/
        void SetRestitutionLimLin(float restitutionLimLin) override;

        /** @copydoc SliderJoint::SetDampingLimLin*/
        void SetDampingLimLin(float dampingLimLin) override;

    private:
        /** @copydoc BulletJoint::BuildJoint */
        void BuildJoint() override;

        /** @copydoc BulletJoint::UpdateJoint */
        void UpdateJoint() override;

        /** @copydoc BulletJoint::ReleaseJoint */
        void ReleaseJoint() override;
    };
}
