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

        /** @copydoc HingeJoint::SetSoftnessLimit */
        void SetSoftnessLimit(float softness);

        /** @copydoc HingeJoint::GetSoftnessLimit */
        float GetSoftnessLimit() const { return _softnessLimit; }

        /** @copydoc HingeJoint::SetBiasLimit */
        void SetBiasLimit(float bias);

        /** @copydoc HingeJoint::GetBiasLimit */
        float GetBiasLimit() const { return _biasLimit; }

        /** @copydoc HingeJoint::SetRelaxationLimit */
        void SetRelaxationLimit(float relaxation);

        /** @copydoc HingeJoint::GetRelaxationLimit */
        float GetRelaxationLimit() const { return _relaxationLimit; }

        /** @copydoc HingeJoint::SetHighLimit */
        void SetHighLimit(Degree highLimit);

        /** @copydoc HingeJoint::GetHighLimit */
        const Degree GetHighLimit() const { return _highLimit; }

        /** @copydoc HingeJoint::SetLowLimit */
        void SetLowLimit(Degree lowLimit);

        /** @copydoc HingeJoint::GetLowLimit */
        const Degree GetLowLimit() const { return _lowLimit; }

        /** @copydoc HingeJoint::SetAngularOnly */
        void SetAngularOnly(bool angularOnly);

        /** @copydoc HingeJoint::GetAngularOnly */
        bool GetAngularOnly() const { return _angularOnly; }

        /** @copydoc HingeJoint::SetMotorEnabled */
        void SetMotorEnabled(bool motorEnabled);

        /** @copydoc HingeJoint::GetMotorEnabled */
        bool GetMotorEnabled() const { return _motorEnabled; }

        /** @copydoc HingeJoint::SetMaxMotorImpulse */
        void SetMaxMotorImpulse(float motorImpulse);

        /** @copydoc HingeJoint::GetMaxMotorImpulse */
        bool GetMaxMotorImpulse() const { return _motorImpulse; }

        /** @copydoc HingeJoint::SetMotorVelocity */
        void SetMotorVelocity(float motorVelocity);

        /** @copydoc HingeJoint::GetMotorVelocity */
        bool GetMotorVelocity() const { return _motorVelocity; }

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
        float _softnessLimit = 1.0f;
        float _biasLimit = 0.3f;
        float _relaxationLimit = 1.0f;
        Degree _highLimit;
        Degree _lowLimit;
        bool _angularOnly = false;
        bool _motorEnabled = false;
        float _motorImpulse = 0.0f;
        float _motorVelocity = 0.0f;
    };
}
