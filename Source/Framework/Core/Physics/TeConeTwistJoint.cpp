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

    void ConeTwistJoint::SetLimitSoftness(float softness)
    {
        _limitSoftness = Math::Clamp(softness, 0.0f, 1.0f);
    }

    void ConeTwistJoint::SetLimitBias(float bias)
    {
        _limitBias = Math::Clamp(bias, 0.0f, 1.0f);
    }

    void ConeTwistJoint::SetLimitRelaxation(float relaxation)
    {
        _limitRelaxation = Math::Clamp(relaxation, 0.0f, 1.0f);
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
