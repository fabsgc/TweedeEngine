#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeHingeJoint.h"
#include "Components/TeCJoint.h"

namespace te
{
    /**
     * @copydoc	HingeJoint
     *
     * @note	Wraps HingeJoint as a Component.
     */
    class TE_CORE_EXPORT CHingeJoint : public CJoint
    {
    public:
        CHingeJoint(const HSceneObject& parent);

        /** Return Component type */
        static UINT32 GetComponentType() { return TID_CHingeJoint; }

        /** @copydoc Component::Clone */
        void Clone(const HHingeJoint& c);

        /** @copydoc HingeJoint::SetLimitSoftness */
        virtual void SetLimitSoftness(float softness);

        /** @copydoc HingeJoint::GetLimitSoftness */
        const float GetLimitSoftness() const { return _limitSoftness; }

        /** @copydoc HingeJoint::SetLimitBias */
        virtual void SetLimitBias(float bias);

        /** @copydoc HingeJoint::GetLimitBias */
        const float GetLimitBias() const { return _limitBias; }

        /** @copydoc HingeJoint::SetLimitRelaxation */
        virtual void SetLimitRelaxation(float relaxation);

        /** @copydoc HingeJoint::GetLimitRelaxation */
        const float GetLimitRelaxation() const { return _limitRelaxation; }

        /** @copydoc HingeJoint::SetLimitHigh */
        virtual void SetLimitHigh(Degree limitHigh);

        /** @copydoc HingeJoint::GetLimitHigh */
        const Degree GetLimitHigh() const { return _limitHigh; }

        /** @copydoc HingeJoint::SetLimitLow */
        virtual void SetLimitLow(Degree limitLow);

        /** @copydoc HingeJoint::GetLimitLow */
        const Degree GetLimitLow() const { return _limitLow; }

        /** @copydoc HingeJoint::SetAngularOnly */
        virtual void SetAngularOnly(bool angularOnly);

        /** @copydoc HingeJoint::GetAngularOnly */
        const bool GetAngularOnly() const { return _angularOnly; }

        /** @copydoc HingeJoint::SetMotorEnabled */
        virtual void SetMotorEnabled(bool motorEnabled);

        /** @copydoc HingeJoint::GetMotorEnabled */
        const bool GetMotorEnabled() const { return _motorEnabled; }

        /** @copydoc HingeJoint::SetMaxMotorImpulse */
        virtual void SetMaxMotorImpulse(float motorImpulse);

        /** @copydoc HingeJoint::GetMaxMotorImpulse */
        const bool GetMaxMotorImpulse() const { return _motorImpulse; }

        /** @copydoc HingeJoint::SetMotorVelocity */
        virtual void SetMotorVelocity(float motorVelocity);

        /** @copydoc HingeJoint::GetMotorVelocity */
        const bool GetMotorVelocity() const { return _motorVelocity; }

    protected:
        friend class SceneObject;

        /** @copydoc Component::OnEnabled */
        void OnEnabled() override;

        /** @copydoc CJoint::CreateInternal */
        SPtr<Joint> CreateInternal() override;

        /**	Returns the Hinge joint that this component wraps. */
        HingeJoint* GetInternal() const { return static_cast<HingeJoint*>(_internal.get()); }

    protected:
        CHingeJoint(); // Serialization only

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
