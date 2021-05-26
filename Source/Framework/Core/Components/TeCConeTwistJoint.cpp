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
        _softnessLimit = c->_softnessLimit;
        _biasLimit = c->_biasLimit;
        _relaxationLimit = c->_relaxationLimit;
        _swingSpan1 = c->_swingSpan1;
        _swingSpan2 = c->_swingSpan2;
        _twistSpan = c->_twistSpan;
        _angularOnly = c->_angularOnly;
        _motorEnabled = c->_motorEnabled;
        _motorImpulse = c->_motorImpulse;
    }

    void CConeTwistJoint::OnEnabled()
    {
        CJoint::OnEnabled();

        if (_internal)
        {
            SetSoftnessLimit(_softnessLimit);
            SetBiasLimit(_biasLimit);
            SetRelaxationLimit(_relaxationLimit);
            SetSwingSpan1(_swingSpan1);
            SetSwingSpan2(_swingSpan2);
            SetTwistSpan(_twistSpan);
            SetAngularOnly(_angularOnly);
            SetMotorEnabled(_motorEnabled);
            SetMaxMotorImpulse(_motorImpulse);
        }
    }

    void CConeTwistJoint::SetDamping(float damping)
    {
        if (_damping == damping)
            return;

        _damping = Math::Clamp(damping, 0.0f, FLT_MAX);

        if (_internal != nullptr)
            std::static_pointer_cast<ConeTwistJoint>(_internal)->SetDamping(damping);
    }

    void CConeTwistJoint::SetSoftnessLimit(float softness)
    {
        if (_softnessLimit == softness)
            return;

        _softnessLimit = Math::Clamp(softness, 0.0f, 1.0f);

        if (_internal != nullptr)
            std::static_pointer_cast<ConeTwistJoint>(_internal)->SetSoftnessLimit(softness);
    }

    void CConeTwistJoint::SetBiasLimit(float bias)
    {
        if (_biasLimit == bias)
            return;

        _biasLimit = Math::Clamp(bias, 0.0f, 1.0f);

        if (_internal != nullptr)
            std::static_pointer_cast<ConeTwistJoint>(_internal)->SetBiasLimit(bias);
    }

    void CConeTwistJoint::SetRelaxationLimit(float relaxation)
    {
        if (_relaxationLimit == relaxation)
            return;

        _relaxationLimit = Math::Clamp(relaxation, 0.0f, 1.0f);

        if (_internal != nullptr)
            std::static_pointer_cast<ConeTwistJoint>(_internal)->SetRelaxationLimit(relaxation);
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
