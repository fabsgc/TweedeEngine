#pragma once

#include "TeCorePrerequisites.h"
#include "Physics/TeSliderJoint.h"
#include "Components/TeCJoint.h"

namespace te
{
    /**
     * @copydoc	SliderJoint
     *
     * @note	Wraps SliderJoint as a Component.
     */
    class TE_CORE_EXPORT CSliderJoint : public CJoint
    {
    public:
        CSliderJoint(const HSceneObject& parent);

        /** Return Component type */
        static UINT32 GetComponentType() { return TypeID_Core::TID_CSliderJoint; }

        /** @copydoc Component::Clone */
        void Clone(const HSliderJoint& c);

        /** @copydoc SetLowerLinLimit */
        void SetLowerLinLimit(float lowerLinLimit);

        /** @copydoc GetLowerLinLimit */
        float GetLowerLinLimit() const { return _lowerLinLimit; }

        /** @copydoc SetUpperLinLimit */
        void SetUpperLinLimit(float upperLinLimit);

        /** @copydoc GetUpperLinLimit */
        float GetUpperLinLimit() const { return _upperLinLimit; }

        /** @copydoc SetLowerAngLimit */
        void SetLowerAngLimit(Degree lowerAngLimit);

        /** @copydoc GetLowerAngLimit */
        const Degree GetLowerAngLimit() const { return _lowerAngLimit; }

        /** @copydoc SetUpperAngLimit */
        void SetUpperAngLimit(Degree upperAngLimit);

        /** @copydoc GetUpperAngLimit */
        const Degree GetUpperAngLimit() const { return _upperAngLimit; }

        /** @copydoc SetSoftnessDirLin */
        void SetSoftnessDirLin(float softnessDirLin);

        /** @copydoc GetSoftnessDirLin */
        float GetSoftnessDirLin() const { return _softnessDirLin; }

        /** @copydoc SetRestitutionDirLin */
        virtual void SetRestitutionDirLin(float restitutionDirLin);

        /** @copydoc GetRestitutionDirLin */
        float GetRestitutionDirLin() const { return _restitutionDirLin; }

        /** @copydoc SetDampingDirLin */
        void SetDampingDirLin(float dampingDirLin);

        /** @copydoc GetDampingDirLin */
        float GetDampingDirLin() const { return _dampingDirLin; }

        /** @copydoc SetSoftnessDirAng */
        void SetSoftnessDirAng(float softnessDirAng);

        /** @copydoc GetSoftnessDirAng */
        float GetSoftnessDirAng() const { return _softnessDirAng; }

        /** @copydoc SetRestitutionDirAng */
        void SetRestitutionDirAng(float restitutionDirAng);

        /** @copydoc GetRestitutionDirAng */
        float GetRestitutionDirAng() const { return _restitutionDirAng; }

        /** @copydoc SetDampingDirAng */
        void SetDampingDirAng(float dampingDirAng);

        /** @copydoc GetDampingDirAng */
        float GetDampingDirAng() const { return _dampingDirAng; }

        /** @copydoc SetSoftnessLimLin */
        void SetSoftnessLimLin(float softnessLimLin);

        /** @copydoc GetSoftnessLimLin */
        float GetSoftnessLimLin() const { return _softnessLimLin; }

        /** @copydoc SetRestitutionLimLin */
        void SetRestitutionLimLin(float restitutionLimLin);

        /** @copydoc GetRestitutionLimLin */
        float GetRestitutionLimLin() const { return _restitutionLimLin; }

        /** @copydoc SetDampingLimLin */
        void SetDampingLimLin(float dampingLimLin);

        /** @copydoc GetDampingLimLin */
        float GetDampingLimLin() const { return _dampingLimLin; }

        /** @copydoc SetSoftnessLimAng */
        void SetSoftnessLimAng(float softnessLimAng);

        /** @copydoc GetSoftnessLimAng */
        float GetSoftnessLimAng() const { return _softnessLimAng; }

        /** @copydoc SetRestitutionLimAng */
        void SetRestitutionLimAng(float restitutionLimAng);

        /** @copydoc GetRestitutionLimAng */
        float GetRestitutionLimAng() const { return _restitutionLimAng; }

        /** @copydoc SetDampingLimAng */
        void SetDampingLimAng(float dampingLimAng);

        /** @copydoc GetDampingLimAng */
        float GetDampingLimAng() const { return _dampingLimAng; }

        /** @copydoc SetSoftnessOrthoLin */
        void SetSoftnessOrthoLin(float softnessOrthoLin);

        /** @copydoc GetSoftnessOrthoLin */
        float GetSoftnessOrthoLin() const { return _softnessOrthoLin; }

        /** @copydoc SetRestitutionOrthoLin */
        void SetRestitutionOrthoLin(float restitutionOrthoLin);

        /** @copydoc GetRestitutionOrthoLin */
        float GetRestitutionOrthoLin() const { return _restitutionOrthoLin; }

        /** @copydoc SetDampingOrthoLin */
        void SetDampingOrthoLin(float dampingOrthoLin);

        /** @copydoc GetDampingOrthoLin */
        float GetDampingOrthoLin() const { return _dampingOrthoLin; }

        /** @copydoc SetSoftnessOrthoAng */
        void SetSoftnessOrthoAng(float softnessOrthoAng);

        /** @copydoc GetSoftnessOrthoAng */
        float GetSoftnessOrthoAng() const { return _softnessOrthoAng; }

        /** @copydoc SetRestitutionOrthoAng */
        void SetRestitutionOrthoAng(float restitutionOrthoAng);

        /** @copydoc GetRestitutionOrthoAng */
        float GetRestitutionOrthoAng() const { return _restitutionOrthoAng; }

        /** @copydoc SetDampingOrthoAng */
        void SetDampingOrthoAng(float dampingOrthoAng);

        /** @copydoc GetDampingOrthoAng */
        float GetDampingOrthoAng() const { return _dampingOrthoAng; }

        /** @copydoc SetPoweredLinMotor */
        void SetPoweredLinMotor(bool poweredLinMotor);

        /** @copydoc GetPoweredLinMotor */
        bool GetPoweredLinMotor() const { return _poweredLinMotor; }

        /** @copydoc SetTargetLinMotorVelocity */
        void SetTargetLinMotorVelocity(float targetLinMotorVelocity);

        /** @copydoc GetTargetLinMotorVelocity */
        float GetTargetLinMotorVelocity() const { return _targetLinMotorVelocity; }

        /** @copydoc SetMaxLinMotorForce */
        void SetMaxLinMotorForce(float maxLinMotorForce);

        /** @copydoc GetMaxLinMotorForce */
        float GetMaxLinMotorForce() const { return _maxLinMotorForce; }

        /** @copydoc SetPoweredAngMotor */
        void SetPoweredAngMotor(bool poweredAngMotor);

        /** @copydoc GetPoweredAngMotor */
        bool GetPoweredAngMotor() const { return _poweredAngMotor; }

        /** @copydoc SetTargetAngMotorVelocity */
        void SetTargetAngMotorVelocity(float targetAngMotorVelocity);

        /** @copydoc GetTargetAngMotorVelocity */
        float GetTargetAngMotorVelocity() const { return _targetAngMotorVelocity; }

        /** @copydoc SetMaxAngMotorForce */
        void SetMaxAngMotorForce(float maxAngMotorForce);

        /** @copydoc GetMaxAngMotorForce */
        float GetMaxAngMotorForce() const { return _maxAngMotorForce; }

    protected:
        friend class SceneObject;

        /** @copydoc Component::OnEnabled */
        void OnEnabled() override;

        /** @copydoc CJoint::CreateInternal */
        SPtr<Joint> CreateInternal() override;

        /**	Returns the Slider joint that this component wraps. */
        SliderJoint* GetInternal() const { return static_cast<SliderJoint*>(_internal.get()); }

    protected:
        CSliderJoint(); // Serialization only

    protected:
        float _lowerLinLimit = 1.0f;
        float _upperLinLimit = -1.0f;
        Degree _lowerAngLimit;
        Degree _upperAngLimit;

        float _softnessDirLin = 1.0f;
        float _restitutionDirLin = 0.7f;
        float _dampingDirLin = 0.0f;
        float _softnessDirAng = 1.0f;
        float _restitutionDirAng = 0.7f;
        float _dampingDirAng = 0.0f;
        float _softnessLimLin = 1.0f;
        float _restitutionLimLin = 0.7f;
        float _dampingLimLin = 1.0f;
        float _softnessLimAng = 1.0f;
        float _restitutionLimAng = 0.7f;
        float _dampingLimAng = 1.0f;
        float _softnessOrthoLin = 1.0f;
        float _restitutionOrthoLin = 0.7f;
        float _dampingOrthoLin = 1.0f;
        float _softnessOrthoAng = 1.0f;
        float _restitutionOrthoAng = 0.7f;
        float _dampingOrthoAng = 1.0f;
        bool _poweredLinMotor = false;
        float _targetLinMotorVelocity = 0.0f;
        float _maxLinMotorForce = 0.0f;
        bool _poweredAngMotor = false;
        float _targetAngMotorVelocity = 0.0f;
        float _maxAngMotorForce = 0.0f;
    };
}
