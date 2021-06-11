#include "Components/TeCSliderJoint.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CSliderJoint::CSliderJoint()
        : CJoint((UINT32)TID_CSliderJoint)
    {
        SetName("SliderJoint");
    }

    CSliderJoint::CSliderJoint(const HSceneObject& parent)
        : CJoint(parent, (UINT32)TID_CSliderJoint)
    {
        SetName("SliderJoint");
    }

    SPtr<Joint> CSliderJoint::CreateInternal()
    {
        const SPtr<SceneInstance>& scene = SO()->GetScene();
        SPtr<Joint> joint = SliderJoint::Create(*scene->GetPhysicsScene());
        joint->SetOwner(PhysicsOwnerType::Component, this);

        return joint;
    }

    void CSliderJoint::Clone(const HSliderJoint& c)
    {
        CJoint::Clone(static_object_cast<CJoint>(c));

        _lowerLinLimit = c->_lowerLinLimit;
        _upperLinLimit = c->_upperLinLimit;
        _lowerAngLimit = c->_lowerAngLimit;
        _upperAngLimit = c->_upperAngLimit;

        _softnessDirLin = c->_softnessDirLin;
        _restitutionDirLin = c->_restitutionDirLin;
        _dampingDirLin = c->_dampingDirLin;
        _softnessDirAng = c->_softnessDirAng;
        _restitutionDirAng = c->_restitutionDirAng;
        _dampingDirAng = c->_dampingDirAng;
        _softnessLimLin = c->_softnessLimLin;
        _restitutionLimLin = c->_restitutionLimLin;
        _dampingLimLin = c->_dampingLimLin;
        _softnessLimAng = c->_softnessLimAng;
        _restitutionLimAng = c->_restitutionLimAng;
        _dampingLimAng = c->_dampingLimAng;
        _softnessOrthoLin = c->_softnessOrthoLin;
        _restitutionOrthoLin = c->_restitutionOrthoLin;
        _dampingOrthoLin = c->_dampingOrthoLin;
        _softnessOrthoAng = c->_softnessOrthoAng;
        _restitutionOrthoAng = c->_restitutionOrthoAng;
        _dampingOrthoAng = c->_dampingOrthoAng;
        _poweredLinMotor = c->_poweredLinMotor;
        _targetLinMotorVelocity = c->_targetLinMotorVelocity;
        _maxLinMotorForce = c->_maxLinMotorForce;
        _poweredAngMotor = c->_poweredAngMotor;
        _targetAngMotorVelocity = c->_targetAngMotorVelocity;
        _maxAngMotorForce = c->_maxAngMotorForce;
    }

    void CSliderJoint::OnEnabled()
    {
        CJoint::OnEnabled();

        if (_internal)
        {
            SetLowerLinLimit(_lowerLinLimit);
            SetUpperLinLimit(_upperLinLimit);
            SetLowerAngLimit(_lowerAngLimit);
            SetUpperAngLimit(_upperAngLimit);

            SetSoftnessDirLin(_softnessDirLin);
            SetRestitutionDirLin(_restitutionDirLin);
            SetDampingDirLin(_dampingDirLin);
            SetSoftnessDirAng(_softnessDirAng);
            SetRestitutionDirAng(_restitutionDirAng);
            SetDampingDirAng(_dampingDirAng);
            SetSoftnessLimLin(_softnessLimLin);
            SetRestitutionLimLin(_restitutionLimLin);
            SetDampingLimLin(_dampingLimLin);
            SetSoftnessLimAng(_softnessLimAng);
            SetRestitutionLimAng(_restitutionLimAng);
            SetDampingLimAng(_dampingLimAng);
            SetSoftnessOrthoLin(_softnessOrthoLin);
            SetRestitutionOrthoLin(_restitutionOrthoLin);
            SetDampingOrthoLin(_dampingOrthoLin);
            SetSoftnessOrthoAng(_softnessOrthoAng);
            SetRestitutionOrthoAng(_restitutionOrthoAng);
            SetDampingOrthoAng(_dampingOrthoAng);
            SetPoweredLinMotor(_poweredLinMotor);
            SetTargetLinMotorVelocity(_targetLinMotorVelocity);
            SetMaxLinMotorForce(_maxLinMotorForce);
            SetPoweredAngMotor(_poweredAngMotor);
            SetTargetAngMotorVelocity(_targetAngMotorVelocity);
            SetMaxAngMotorForce(_maxAngMotorForce);
        }
    }

    void CSliderJoint::SetLowerLinLimit(float lowerLinLimit)
    {
        if (_lowerLinLimit == lowerLinLimit)
            return;

        _lowerLinLimit = Math::Clamp(lowerLinLimit, 0.0f, 1.0f);

        if (_internal != nullptr)
            std::static_pointer_cast<SliderJoint>(_internal)->SetLowerLinLimit(_lowerLinLimit);
    }

    void CSliderJoint::SetUpperLinLimit(float upperLinLimit)
    {
        if (_upperLinLimit == upperLinLimit)
            return;

        _upperLinLimit = Math::Clamp(upperLinLimit, 0.0f, 1.0f);

        if (_internal != nullptr)
            std::static_pointer_cast<SliderJoint>(_internal)->SetLowerLinLimit(_upperLinLimit);
    }

    void CSliderJoint::SetLowerAngLimit(Degree lowerAngLimit)
    {
        if (_lowerAngLimit == lowerAngLimit)
            return;

        _lowerAngLimit = Math::Clamp(lowerAngLimit, Degree(0), Degree(90));

        if (_internal != nullptr)
            std::static_pointer_cast<SliderJoint>(_internal)->SetLowerAngLimit(_lowerAngLimit);
    }

    void CSliderJoint::SetUpperAngLimit(Degree upperAngLimit)
    {
        if (_upperAngLimit == upperAngLimit)
            return;

        _upperAngLimit = Math::Clamp(upperAngLimit, Degree(0), Degree(90));

        if (_internal != nullptr)
            std::static_pointer_cast<SliderJoint>(_internal)->SetUpperAngLimit(_upperAngLimit);
    }

    void CSliderJoint::SetSoftnessDirLin(float softnessDirLin)
    {
        _softnessDirLin = softnessDirLin;

        if (_softnessDirLin == softnessDirLin)
            return;

        _softnessDirLin = softnessDirLin;

        if (_internal != nullptr)
            std::static_pointer_cast<SliderJoint>(_internal)->SetSoftnessDirLin(softnessDirLin);
    }

    void CSliderJoint::SetRestitutionDirLin(float restitutionDirLin)
    {
        _restitutionDirLin = restitutionDirLin;

        if (_restitutionDirLin == restitutionDirLin)
            return;

        _restitutionDirLin = restitutionDirLin;

        if (_internal != nullptr)
            std::static_pointer_cast<SliderJoint>(_internal)->SetRestitutionDirLin(restitutionDirLin);
    }

    void CSliderJoint::SetDampingDirLin(float dampingDirLin)
    {
        _dampingDirLin = dampingDirLin;

        if (_dampingDirLin == dampingDirLin)
            return;

        _dampingDirLin = dampingDirLin;

        if (_internal != nullptr)
            std::static_pointer_cast<SliderJoint>(_internal)->SetDampingDirLin(dampingDirLin);
    }

    void CSliderJoint::SetSoftnessDirAng(float softnessDirAng)
    {
        _softnessDirAng = softnessDirAng;

        if (_softnessDirAng == softnessDirAng)
            return;

        _softnessDirAng = softnessDirAng;

        if (_internal != nullptr)
            std::static_pointer_cast<SliderJoint>(_internal)->SetSoftnessDirAng(softnessDirAng);
    }

    void CSliderJoint::SetRestitutionDirAng(float restitutionDirAng)
    {
        _restitutionDirAng = restitutionDirAng;

        if (_restitutionDirAng == restitutionDirAng)
            return;

        _restitutionDirAng = restitutionDirAng;

        if (_internal != nullptr)
            std::static_pointer_cast<SliderJoint>(_internal)->SetRestitutionDirAng(restitutionDirAng);
    }

    void CSliderJoint::SetDampingDirAng(float dampingDirAng)
    {
        _dampingDirAng = dampingDirAng;

        if (_dampingDirAng == dampingDirAng)
            return;

        _dampingDirAng = dampingDirAng;

        if (_internal != nullptr)
            std::static_pointer_cast<SliderJoint>(_internal)->SetDampingDirAng(dampingDirAng);
    }

    void CSliderJoint::SetSoftnessLimLin(float softnessLimLin)
    {
        _softnessLimLin = softnessLimLin;

        if (_softnessLimLin == softnessLimLin)
            return;

        _softnessLimLin = softnessLimLin;

        if (_internal != nullptr)
            std::static_pointer_cast<SliderJoint>(_internal)->SetSoftnessLimLin(softnessLimLin);
    }

    void CSliderJoint::SetRestitutionLimLin(float restitutionLimLin)
    {
        _restitutionLimLin = restitutionLimLin;

        if (_restitutionLimLin == restitutionLimLin)
            return;

        _restitutionLimLin = restitutionLimLin;

        if (_internal != nullptr)
            std::static_pointer_cast<SliderJoint>(_internal)->SetSoftnessLimLin(restitutionLimLin);
    }

    void CSliderJoint::SetDampingLimLin(float dampingLimLin)
    {
        _dampingLimLin = dampingLimLin;

        if (_dampingLimLin == dampingLimLin)
            return;

        _dampingLimLin = dampingLimLin;

        if (_internal != nullptr)
            std::static_pointer_cast<SliderJoint>(_internal)->SetDampingLimLin(dampingLimLin);
    }

    void CSliderJoint::SetSoftnessLimAng(float softnessLimAng)
    {
        _softnessLimAng = softnessLimAng;

        if (_softnessLimAng == softnessLimAng)
            return;

        _softnessLimAng = softnessLimAng;

        if (_internal != nullptr)
            std::static_pointer_cast<SliderJoint>(_internal)->SetSoftnessLimAng(softnessLimAng);
    }

    void CSliderJoint::SetRestitutionLimAng(float restitutionLimAng)
    {
        _restitutionLimAng = restitutionLimAng;

        if (_restitutionLimAng == restitutionLimAng)
            return;

        _restitutionLimAng = restitutionLimAng;

        if (_internal != nullptr)
            std::static_pointer_cast<SliderJoint>(_internal)->SetRestitutionLimAng(restitutionLimAng);
    }

    void CSliderJoint::SetDampingLimAng(float dampingLimAng)
    {
        _dampingLimAng = dampingLimAng;

        if (_dampingLimAng == dampingLimAng)
            return;

        _dampingLimAng = dampingLimAng;

        if (_internal != nullptr)
            std::static_pointer_cast<SliderJoint>(_internal)->SetDampingLimAng(dampingLimAng);
    }

    void CSliderJoint::SetSoftnessOrthoLin(float softnessOrthoLin)
    {
        _softnessOrthoLin = softnessOrthoLin;

        if (_softnessOrthoLin == softnessOrthoLin)
            return;

        _softnessOrthoLin = softnessOrthoLin;

        if (_internal != nullptr)
            std::static_pointer_cast<SliderJoint>(_internal)->SetSoftnessOrthoLin(softnessOrthoLin);
    }

    void CSliderJoint::SetRestitutionOrthoLin(float restitutionOrthoLin)
    {
        _restitutionOrthoLin = restitutionOrthoLin;

        if (_restitutionOrthoLin == restitutionOrthoLin)
            return;

        _restitutionOrthoLin = restitutionOrthoLin;

        if (_internal != nullptr)
            std::static_pointer_cast<SliderJoint>(_internal)->SetRestitutionOrthoLin(restitutionOrthoLin);
    }

    void CSliderJoint::SetDampingOrthoLin(float dampingOrthoLin)
    {
        _dampingOrthoLin = dampingOrthoLin;

        if (_dampingOrthoLin == dampingOrthoLin)
            return;

        _dampingOrthoLin = dampingOrthoLin;

        if (_internal != nullptr)
            std::static_pointer_cast<SliderJoint>(_internal)->SetDampingOrthoLin(dampingOrthoLin);
    }

    void CSliderJoint::SetSoftnessOrthoAng(float softnessOrthoAng)
    {
        _softnessOrthoAng = softnessOrthoAng;

        if (_softnessOrthoAng == softnessOrthoAng)
            return;

        _softnessOrthoAng = softnessOrthoAng;

        if (_internal != nullptr)
            std::static_pointer_cast<SliderJoint>(_internal)->SetSoftnessOrthoAng(softnessOrthoAng);
    }

    void CSliderJoint::SetRestitutionOrthoAng(float restitutionOrthoAng)
    {
        _restitutionOrthoAng = restitutionOrthoAng;

        if (_restitutionOrthoAng == restitutionOrthoAng)
            return;

        _restitutionOrthoAng = restitutionOrthoAng;

        if (_internal != nullptr)
            std::static_pointer_cast<SliderJoint>(_internal)->SetRestitutionOrthoAng(restitutionOrthoAng);
    }

    void CSliderJoint::SetDampingOrthoAng(float dampingOrthoAng)
    {
        _dampingOrthoAng = dampingOrthoAng;

        if (_dampingOrthoAng == dampingOrthoAng)
            return;

        _dampingOrthoAng = dampingOrthoAng;

        if (_internal != nullptr)
            std::static_pointer_cast<SliderJoint>(_internal)->SetDampingOrthoAng(dampingOrthoAng);
    }

    void CSliderJoint::SetPoweredLinMotor(bool poweredLinMotor)
    {
        _poweredLinMotor = poweredLinMotor;

        if (_poweredLinMotor == poweredLinMotor)
            return;

        _poweredLinMotor = poweredLinMotor;

        if (_internal != nullptr)
            std::static_pointer_cast<SliderJoint>(_internal)->SetPoweredLinMotor(poweredLinMotor);
    }

    void CSliderJoint::SetTargetLinMotorVelocity(float targetLinMotorVelocity)
    {
        _targetLinMotorVelocity = targetLinMotorVelocity;

        if (_targetLinMotorVelocity == targetLinMotorVelocity)
            return;

        _targetLinMotorVelocity = targetLinMotorVelocity;

        if (_internal != nullptr)
            std::static_pointer_cast<SliderJoint>(_internal)->SetTargetLinMotorVelocity(targetLinMotorVelocity);
    }

    void CSliderJoint::SetMaxLinMotorForce(float maxLinMotorForce)
    {
        _maxLinMotorForce = maxLinMotorForce;

        if (_maxLinMotorForce == maxLinMotorForce)
            return;

        _maxLinMotorForce = maxLinMotorForce;

        if (_internal != nullptr)
            std::static_pointer_cast<SliderJoint>(_internal)->SetMaxLinMotorForce(maxLinMotorForce);
    }

    void CSliderJoint::SetPoweredAngMotor(bool poweredAngMotor)
    {
        _poweredAngMotor = poweredAngMotor;

        if (_poweredAngMotor == poweredAngMotor)
            return;

        _poweredAngMotor = poweredAngMotor;

        if (_internal != nullptr)
            std::static_pointer_cast<SliderJoint>(_internal)->SetPoweredAngMotor(poweredAngMotor);
    }

    void CSliderJoint::SetTargetAngMotorVelocity(float targetAngMotorVelocity)
    {
        _targetAngMotorVelocity = targetAngMotorVelocity;

        if (_targetAngMotorVelocity == targetAngMotorVelocity)
            return;

        _targetAngMotorVelocity = targetAngMotorVelocity;

        if (_internal != nullptr)
            std::static_pointer_cast<SliderJoint>(_internal)->SetTargetAngMotorVelocity(targetAngMotorVelocity);
    }

    void CSliderJoint::SetMaxAngMotorForce(float maxAngMotorForce)
    {
        _maxAngMotorForce = maxAngMotorForce;

        if (_maxAngMotorForce == maxAngMotorForce)
            return;

        _maxAngMotorForce = maxAngMotorForce;

        if (_internal != nullptr)
            std::static_pointer_cast<SliderJoint>(_internal)->SetMaxAngMotorForce(maxAngMotorForce);
    }
}
