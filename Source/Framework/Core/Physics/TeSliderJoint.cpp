#include "TeSliderJoint.h"
#include "Physics/TePhysics.h"

namespace te
{ 
    SliderJoint::SliderJoint()
        : _lowerAngLimit(0.0f)
        , _upperAngLimit(0.0f)
    { }

    SPtr<SliderJoint> SliderJoint::Create(PhysicsScene& scene)
    {
        return scene.CreateSliderJoint();
    }

    void SliderJoint::SetLowerLinLimit(float lowerLinLimit)
    {
        _lowerLinLimit = lowerLinLimit;
    }

    void SliderJoint::SetUpperLinLimit(float upperLinLimit)
    {
        _upperLinLimit = upperLinLimit;
    }

    void SliderJoint::SetLowerAngLimit(Degree lowerAngLimit)
    {
        _lowerAngLimit = lowerAngLimit;
    }

    void SliderJoint::SetUpperAngLimit(Degree upperAngLimit)
    {
        _upperAngLimit = upperAngLimit;
    }

    void SliderJoint::SetSoftnessDirLin(float softnessDirLin)
    {
        _softnessDirLin = softnessDirLin;
    }

    void SliderJoint::SetRestitutionDirLin(float restitutionDirLin)
    {
        _restitutionDirLin = restitutionDirLin;
    }

    void SliderJoint::SetDampingDirLin(float dampingDirLin)
    {
        _dampingDirLin = dampingDirLin;
    }

    void SliderJoint::SetSoftnessDirAng(float softnessDirAng)
    {
        _softnessDirAng = softnessDirAng;
    }

    void SliderJoint::SetRestitutionDirAng(float restitutionDirAng)
    {
        _restitutionDirAng = restitutionDirAng;
    }

    void SliderJoint::SetDampingDirAng(float dampingDirAng)
    {
        _dampingDirAng = dampingDirAng;
    }

    void SliderJoint::SetSoftnessLimLin(float softnessLimLin)
    {
        _softnessLimLin = softnessLimLin;
    }

    void SliderJoint::SetRestitutionLimLin(float restitutionLimLin)
    {
        _restitutionLimLin = restitutionLimLin;
    }

    void SliderJoint::SetDampingLimLin(float dampingLimLin)
    {
        _dampingLimLin = dampingLimLin;
    }

    void SliderJoint::SetSoftnessLimAng(float softnessLimAng)
    {
        _softnessLimAng = softnessLimAng;
    }

    void SliderJoint::SetRestitutionLimAng(float restitutionLimAng)
    {
        _restitutionLimAng = restitutionLimAng;
    }

    void SliderJoint::SetDampingLimAng(float dampingLimAng)
    {
        _dampingLimAng = dampingLimAng;
    }

    void SliderJoint::SetSoftnessOrthoLin(float softnessOrthoLin)
    {
        _softnessOrthoLin = softnessOrthoLin;
    }

    void SliderJoint::SetRestitutionOrthoLin(float restitutionOrthoLin)
    {
        _restitutionOrthoLin = restitutionOrthoLin;
    }

    void SliderJoint::SetDampingOrthoLin(float dampingOrthoLin)
    {
        _dampingOrthoLin = dampingOrthoLin;
    }

    void SliderJoint::SetSoftnessOrthoAng(float softnessOrthoAng)
    {
        _softnessOrthoAng = softnessOrthoAng;
    }

    void SliderJoint::SetRestitutionOrthoAng(float restitutionOrthoAng)
    {
        _restitutionOrthoAng = restitutionOrthoAng;
    }

    void SliderJoint::SetDampingOrthoAng(float dampingOrthoAng)
    {
        _dampingOrthoAng = dampingOrthoAng;
    }

    void SliderJoint::SetPoweredLinMotor(bool poweredLinMotor)
    {
        _poweredLinMotor = poweredLinMotor;
    }

    void SliderJoint::SetTargetLinMotorVelocity(float targetLinMotorVelocity)
    {
        _targetLinMotorVelocity = targetLinMotorVelocity;
    }

    void SliderJoint::SetMaxLinMotorForce(float maxLinMotorForce)
    {
        _maxLinMotorForce = maxLinMotorForce;
    }

    void SliderJoint::SetPoweredAngMotor(bool poweredAngMotor)
    {
        _poweredAngMotor = poweredAngMotor;
    }

    void SliderJoint::SetTargetAngMotorVelocity(float targetAngMotorVelocity)
    {
        _targetAngMotorVelocity = targetAngMotorVelocity;
    }

    void SliderJoint::SetMaxAngMotorForce(float maxAngMotorForce)
    {
        _maxAngMotorForce = maxAngMotorForce;
    }
}
