#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeJoint.h"

namespace te
{
    /**
     * To create ragdolls, the conve twist constraint is very useful for limbs like the upper arm. It is a special 
     * point to point constraint that adds cone and twist axis limits. The x-axis serves as twist axis.
     */
    class TE_CORE_EXPORT ConeTwistJoint : public Joint
    {
    public:
        ConeTwistJoint();
        virtual ~ConeTwistJoint() = default;

        /** @copydoc Joint::Update */
        virtual void Update() = 0;

        /** Specify angular damping */
        virtual void SetDamping(float damping);

        /** @copydoc SetDamping */
        const float GetDamping() const { return _damping; }

        /** 
         * Describes % of limits where movement is free.
         * Beyond this softness %, the limit is gradually enforced until the "hard" (1.0) limit is reached.
         * From 0 to 1. Recommend ~0.8->1
         */
        virtual void SetSoftnessLimit(float softness);

        /** @copydoc SetSoftnessLimit */
        const float GetSoftnessLimit() const { return _softnessLimit; }

        /**
         * Strength with which constraint resists zeroth order (angular, not angular velocity) limit violation.
         * From 0 to 1. Recommend 0.3 +/-0.3 or so.
        */
        virtual void SetBiasLimit(float bias);

        /** @copydoc SetBiasLimit */
        const float GetBiasLimit() const { return _biasLimit; }

        /** 
         * the lower the value, the less the constraint will fight velocities which violate the angular limits.
         * From 0 to 1. Recommend to stay near 1.
         */
        virtual void SetRelaxationLimit(float relaxation);

        /** @copydoc SetRelaxationLimit */
        const float GetRelaxationLimit() const { return _relaxationLimit; }

        /**
         * Specify twist limit of the joint
         *
         * When the axis of symmetry of the cone is at
         * its default of Vector3::UNIT_X, swingSpan1 is the half angle of
         * the apex of the isosceles triangle made by projecting the cone
         * onto the XY plane. Value can range from 0 to 90
         */
        virtual void SetSwingSpan1(Degree deg);

        /** @copydoc SetSwingSpan1 */
        const Degree GetSwingSpan1() const { return _swingSpan1; }

        /**
         * Specify twist limit of the joint
         *
         * When the axis of symmetry of the cone is at
         * its default of Vector3::UNIT_X, swingSpan2 is the half angle of
         * the apex of the isosceles triangle made by projecting the cone
         * onto the XZ plane. Value can range from 0 to 90
         */
        virtual void SetSwingSpan2(Degree deg);

        /** @copydoc SetSwingSpan2 */
        const Degree GetSwingSpan2() const { return _swingSpan2; }

        /**
         * Specify twist limit of the joint
         *
         * Maximum angle in radians that the two
         * connected bodies can twist along the axis of symmetry of the
         * cone. Value can range from 0 to 360
         */
        virtual void SetTwistSpan(Degree deg);

        /** @copydoc SetTwistSpan */
        const Degree GetTwistSpan() const { return _twistSpan; }

        /** Specify if this joint affect angular properties only */
        virtual void SetAngularOnly(bool angularOnly);

        /** @copydoc SetAngularOnly */
        const bool GetAngularOnly() const { return _angularOnly; }

        /** Turns the motor on the X axis on and off */
        virtual void SetMotorEnabled(bool motorEnabled);

        /** @copydoc SetMotorEnabled */
        const bool GetMotorEnabled() const { return _motorEnabled; }

        /** Turns the motor on the X axis on and off */
        virtual void SetMaxMotorImpulse(float motorImpulse);

        /** @copydoc SetMaxMotorImpulse */
        const bool GetMaxMotorImpulse() const { return _motorImpulse; }

        /**
         * Creates a new cone twist joint.
         *
         * @param[in]	scene		Scene to which to add the joint.
         * @param[in]	desc		Settings describing the joint.
         */
        static SPtr<ConeTwistJoint> Create(PhysicsScene& scene);

    protected:
        float _damping = 0.0f;
        float _softnessLimit = 1.0f;
        float _biasLimit = 0.3f;
        float _relaxationLimit = 1.0f;
        Degree _swingSpan1;
        Degree _swingSpan2;
        Degree _twistSpan;
        bool _angularOnly = false;
        bool _motorEnabled = false;
        float _motorImpulse = 0.0f;
    };
}
