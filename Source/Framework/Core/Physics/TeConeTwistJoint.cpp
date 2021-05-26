#include "TeConeTwistJoint.h"
#include "Physics/TePhysics.h"

#include <cfloat>

namespace te
{
    ConeTwistJoint::ConeTwistJoint()
        : _swingSpan1(0.0f)
        , _swingSpan2(0.0f)
        , _twistSpan(0.0f)
    { }

    void ConeTwistJoint::SetDamping(float damping)
    {
        _damping = Math::Clamp(damping, 0.0f, FLT_MAX);
    }

    void ConeTwistJoint::SetSoftnessLimit(float softness)
    {
        _softnessLimit = Math::Clamp(softness, 0.0f, 1.0f);
    }

    void ConeTwistJoint::SetBiasLimit(float bias)
    {
        _biasLimit = Math::Clamp(bias, 0.0f, 1.0f);
    }

    void ConeTwistJoint::SetRelaxationLimit(float relaxation)
    {
        _relaxationLimit = Math::Clamp(relaxation, 0.0f, 1.0f);
    }

    void ConeTwistJoint::SetSwingSpan1(Degree deg)
    {
        _swingSpan1 = Math::Clamp(deg, Degree(0), Degree(90));
    }

    void ConeTwistJoint::SetSwingSpan2(Degree deg)
    {
        _swingSpan2 = Math::Clamp(deg, Degree(0), Degree(90));
    }

    void ConeTwistJoint::SetTwistSpan(Degree deg)
    {
        _twistSpan = Math::Clamp(deg, Degree(0), Degree(360));
    }

    void ConeTwistJoint::SetAngularOnly(bool angularOnly)
    {
        _angularOnly = angularOnly;
    }

    void ConeTwistJoint::SetMotorEnabled(bool motorEnabled)
    {
        _motorEnabled = motorEnabled;
    }

    void ConeTwistJoint::SetMaxMotorImpulse(float motorImpulse)
    {
        _motorImpulse = motorImpulse;
    }

    SPtr<ConeTwistJoint> ConeTwistJoint::Create(PhysicsScene& scene)
    {
        return scene.CreateConeTwistJoint();
    }
}
