#include "TeHingeJoint.h"
#include "Physics/TePhysics.h"

namespace te
{
    HingeJoint::HingeJoint()
        : _limitHigh(0.0f)
        , _limitLow(0.0f)
    { }

    SPtr<HingeJoint> HingeJoint::Create(PhysicsScene& scene)
    {
        return scene.CreateHingeJoint();
    }

    void HingeJoint::SetLimitSoftness(float softness)
    {
        _limitSoftness = Math::Clamp(softness, 0.0f, 1.0f);
    }

    void HingeJoint::SetLimitBias(float bias)
    {
        _limitBias = Math::Clamp(bias, 0.0f, 1.0f);
    }

    void HingeJoint::SetLimitRelaxation(float relaxation)
    {
        _limitRelaxation = Math::Clamp(relaxation, 0.0f, 1.0f);
    }

    void HingeJoint::SetLimitHigh(Degree limitHigh)
    {
        _limitHigh = limitHigh;
    }

    void HingeJoint::SetLimitLow(Degree limitLow)
    {
        _limitLow = limitLow;
    }

    void HingeJoint::SetAngularOnly(bool angularOnly)
    {
        _angularOnly = angularOnly;
    }

    void HingeJoint::SetMotorEnabled(bool motorEnabled)
    {
        _motorEnabled = motorEnabled;
    }

    void HingeJoint::SetMaxMotorImpulse(float motorImpulse)
    {
        _motorImpulse = motorImpulse;
    }

    void HingeJoint::SetMotorVelocity(float motorVelocity)
    {
        _motorVelocity = motorVelocity;
    }
}
