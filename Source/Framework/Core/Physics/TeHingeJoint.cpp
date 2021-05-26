#include "TeHingeJoint.h"
#include "Physics/TePhysics.h"

namespace te
{
    HingeJoint::HingeJoint()
        : _highLimit(0.0f)
        , _lowLimit(0.0f)
    { }

    SPtr<HingeJoint> HingeJoint::Create(PhysicsScene& scene)
    {
        return scene.CreateHingeJoint();
    }

    void HingeJoint::SetSoftnessLimit(float softness)
    {
        _softnessLimit = Math::Clamp(softness, 0.0f, 1.0f);
    }

    void HingeJoint::SetBiasLimit(float bias)
    {
        _biasLimit = Math::Clamp(bias, 0.0f, 1.0f);
    }

    void HingeJoint::SetRelaxationLimit(float relaxation)
    {
        _relaxationLimit = Math::Clamp(relaxation, 0.0f, 1.0f);
    }

    void HingeJoint::SetHighLimit(Degree highLimit)
    {
        _highLimit = highLimit;
    }

    void HingeJoint::SetLowLimit(Degree lowLimit)
    {
        _lowLimit = lowLimit;
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
