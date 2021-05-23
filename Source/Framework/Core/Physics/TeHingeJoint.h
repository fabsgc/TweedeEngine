#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeJoint.h"

namespace te
{
    /**
     * Hinge joint removes all but a single rotation degree of freedom from its two attached bodies (for example a door
     * hinge).
     */
    class TE_CORE_EXPORT HingeJoint : public Joint
    {
    public:
        HingeJoint();
        virtual ~HingeJoint() = default;

        /** @copydoc Joint::Update */
        virtual void Update() = 0;

        /**
         * Describes % of limits where movement is free.
         * Beyond this softness %, the limit is gradually enforced until the "hard" (1.0) limit is reached.
         * From 0 to 1. Recommend ~0.8->1
         */
        virtual void SetLimitSoftness(float softness);

        /** @copydoc SetLimitSoftness */
        const float GetLimitSoftness() const { return _limitSoftness; }

        /**
         * Strength with which constraint resists zeroth order (angular, not angular velocity) limit violation.
         * From 0 to 1. Recommend 0.3 +/-0.3 or so.
        */
        virtual void SetLimitBias(float bias);

        /** @copydoc SetLimitBias */
        const float GetLimitBias() const { return _limitBias; }

        /**
         * the lower the value, the less the constraint will fight velocities which violate the angular limits.
         * From 0 to 1. Recommend to stay near 1.
         */
        virtual void SetLimitRelaxation(float relaxation);

        /** @copydoc SetLimitRelaxation */
        const float GetLimitRelaxation() const { return _limitRelaxation; }

        /** Angular Constraint Max Z */
        virtual void SetLimitHigh(Degree limitHigh);

        /** @copydoc SetLimitHigh */
        const Degree GetLimitHigh() const { return _limitHigh; }

        /** Angular Constraint Min Z */
        virtual void SetLimitLow(Degree limitLow);

        /** @copydoc SetLimitLow */
        const Degree GetLimitLow() const { return _limitLow; }

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

        /** Turns the motor on the X axis on and off */
        virtual void SetMotorVelocity(float motorVelocity);

        /** @copydoc SetMaxMotorImpulse */
        const bool GetMotorVelocity() const { return _motorVelocity; }

        /**
         * Creates a new hinge joint.
         *
         * @param[in]	scene		Scene to which to add the joint.
         * @param[in]	desc		Settings describing the joint.
         */
        static SPtr<HingeJoint> Create(PhysicsScene& scene);

    protected:
        float _limitSoftness = 1.0f;
        float _limitBias = 0.3f;
        float _limitRelaxation = 1.0f;
        Degree _limitHigh;
        Degree _limitLow;
        bool _angularOnly = false;
        bool _motorEnabled = false;
        float _motorImpulse = 0.0f;
        float _motorVelocity = 0.0f;
    };
}
