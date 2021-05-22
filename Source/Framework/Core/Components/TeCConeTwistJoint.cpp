#include "Components/TeCConeTwistJoint.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CConeTwistJoint::CConeTwistJoint()
        : CJoint((UINT32)TID_CConeTwistJoint)
        , _swingSpan1(0.0f)
        , _swingSpan2(0.0f)
        , _twistSpan(0.0f)
    {
        SetName("ConeTwistJoint");
    }

    CConeTwistJoint::CConeTwistJoint(const HSceneObject& parent)
        : CJoint(parent, (UINT32)TID_CConeTwistJoint)
        , _swingSpan1(0.0f)
        , _swingSpan2(0.0f)
        , _twistSpan(0.0f)
    {
        SetName("ConeTwistJoint");
    }

    SPtr<Joint> CConeTwistJoint::CreateInternal()
    {
        const SPtr<SceneInstance>& scene = SO()->GetScene();
        SPtr<Joint> joint = ConeTwistJoint::Create(*scene->GetPhysicsScene());

        return joint;
    }

    void CConeTwistJoint::Clone(const HConeTwistJoint& c)
    {
        CJoint::Clone(static_object_cast<CJoint>(c));

        _damping = c->_damping;
        _limitSoftness = c->_limitSoftness;
        _limitBias = c->_limitBias;
        _limitRelaxation = c->_limitRelaxation;
        _swingSpan1 = c->_swingSpan1;
        _swingSpan2 = c->_swingSpan2;
        _twistSpan = c->_twistSpan;
        _angularOnly = c->_angularOnly;
    }

    void CConeTwistJoint::SetDamping(float damping)
    {
        if (_damping == damping)
            return;

        _damping = Math::Clamp(damping, 0.0f, FLT_MAX);

        if (_internal != nullptr)
            std::static_pointer_cast<ConeTwistJoint>(_internal)->SetDamping(damping);
    }

    void CConeTwistJoint::SetLimitSoftness(float softness)
    {
        if (_limitSoftness == softness)
            return;

        _limitSoftness = Math::Clamp(softness, 0.0f, 1.0f);

        if (_internal != nullptr)
            std::static_pointer_cast<ConeTwistJoint>(_internal)->SetLimitSoftness(softness);
    }

    void CConeTwistJoint::SetLimitBias(float bias)
    {
        if (_limitBias == bias)
            return;

        _limitBias = Math::Clamp(bias, 0.0f, 1.0f);

        if (_internal != nullptr)
            std::static_pointer_cast<ConeTwistJoint>(_internal)->SetLimitBias(bias);
    }

    void CConeTwistJoint::SetLimitRelaxation(float relaxation)
    {
        if (_limitRelaxation == relaxation)
            return;

        _limitRelaxation = Math::Clamp(relaxation, 0.0f, 1.0f);

        if (_internal != nullptr)
            std::static_pointer_cast<ConeTwistJoint>(_internal)->SetLimitRelaxation(relaxation);
    }

    void CConeTwistJoint::SetSwingSpan1(Degree deg)
    {
        if (_swingSpan1 == deg)
            return;

        _swingSpan1 = Math::Clamp(deg, Degree(0), Degree(90));

        if (_internal != nullptr)
            std::static_pointer_cast<ConeTwistJoint>(_internal)->SetSwingSpan1(deg);
    }

    void CConeTwistJoint::SetSwingSpan2(Degree deg)
    {
        if (_swingSpan2 == deg)
            return;

        _swingSpan2 = Math::Clamp(deg, Degree(0), Degree(90));

        if (_internal != nullptr)
            std::static_pointer_cast<ConeTwistJoint>(_internal)->SetSwingSpan2(deg);
    }

    void CConeTwistJoint::SetTwistSpan(Degree deg)
    {
        if (_twistSpan == deg)
            return;

        _twistSpan = Math::Clamp(deg, Degree(0), Degree(360));

        if (_internal != nullptr)
            std::static_pointer_cast<ConeTwistJoint>(_internal)->SetTwistSpan(deg);
    }

    void CConeTwistJoint::SetAngularOnly(bool angularOnly)
    {
        if (_angularOnly == angularOnly)
            return;

        _angularOnly = angularOnly;

        if (_internal != nullptr)
            std::static_pointer_cast<ConeTwistJoint>(_internal)->SetAngularOnly(angularOnly);
    }

    void CConeTwistJoint::SetMotorEnabled(bool motorEnabled)
    {
        if (_motorEnabled == motorEnabled)
            return;

        _motorEnabled = motorEnabled;

        if (_internal != nullptr)
            std::static_pointer_cast<ConeTwistJoint>(_internal)->SetMotorEnabled(motorEnabled);
    }

    void CConeTwistJoint::SetMaxMotorImpulse(float motorImpulse)
    {
        if (_motorImpulse == motorImpulse)
            return;

        _motorImpulse = motorImpulse;

        if (_internal != nullptr)
            std::static_pointer_cast<ConeTwistJoint>(_internal)->SetMaxMotorImpulse(motorImpulse);
    }
}
