#include "Components/TeCHingeJoint.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CHingeJoint::CHingeJoint()
        : CJoint((UINT32)TID_CHingeJoint)
    {
        SetName("HingeJoint");
    }

    CHingeJoint::CHingeJoint(const HSceneObject& parent)
        : CJoint(parent, (UINT32)TID_CHingeJoint)
    {
        SetName("HingeJoint");
    }

    SPtr<Joint> CHingeJoint::CreateInternal()
    {
        const SPtr<SceneInstance>& scene = SO()->GetScene();
        SPtr<Joint> joint = HingeJoint::Create(*scene->GetPhysicsScene());
        joint->SetOwner(PhysicsOwnerType::Component, this);

        return joint;
    }

    bool CHingeJoint::Clone(const HComponent& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        return Clone(static_object_cast<CHingeJoint>(c), suffix);
    }

    bool CHingeJoint::Clone(const HHingeJoint& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        if (CJoint::Clone(static_object_cast<CJoint>(c), suffix))
        {
            _softnessLimit = c->_softnessLimit;
            _biasLimit = c->_biasLimit;
            _relaxationLimit = c->_relaxationLimit;
            _lowLimit = c->_lowLimit;
            _highLimit = c->_highLimit;
            _angularOnly = c->_angularOnly;
            _motorEnabled = c->_motorEnabled;
            _motorImpulse = c->_motorImpulse;
            _motorVelocity = c->_motorVelocity;

            return true;
        }

        return false;
    }

    void CHingeJoint::OnEnabled()
    {
        CJoint::OnEnabled();

        if (_internal)
        {
            SetSoftnessLimit(_softnessLimit);
            SetBiasLimit(_biasLimit);
            SetRelaxationLimit(_relaxationLimit);
            SetHighLimit(_highLimit);
            SetLowLimit(_lowLimit);
            SetAngularOnly(_angularOnly);
            SetMotorEnabled(_motorEnabled);
            SetMaxMotorImpulse(_motorImpulse);
            SetMotorVelocity(_motorVelocity);
        }
    }

    void CHingeJoint::SetSoftnessLimit(float softness)
    {
        if (_softnessLimit == softness)
            return;

        _softnessLimit = Math::Clamp(softness, 0.0f, 1.0f);

        if (_internal != nullptr)
            std::static_pointer_cast<HingeJoint>(_internal)->SetSoftnessLimit(_softnessLimit);
    }

    void CHingeJoint::SetBiasLimit(float bias)
    {
        if (_biasLimit == bias)
            return;

        _biasLimit = Math::Clamp(bias, 0.0f, 1.0f);

        if (_internal != nullptr)
            std::static_pointer_cast<HingeJoint>(_internal)->SetBiasLimit(_biasLimit);
    }

    void CHingeJoint::SetRelaxationLimit(float relaxation)
    {
        if (_relaxationLimit == relaxation)
            return;

        _relaxationLimit = Math::Clamp(relaxation, 0.0f, 1.0f);

        if (_internal != nullptr)
            std::static_pointer_cast<HingeJoint>(_internal)->SetRelaxationLimit(_relaxationLimit);
    }

    void CHingeJoint::SetHighLimit(Degree highLimit)
    {
        if (_highLimit == highLimit)
            return;

        _highLimit = highLimit;

        if (_highLimit < _lowLimit)
            _lowLimit = _lowLimit;

        if (_internal != nullptr)
            std::static_pointer_cast<HingeJoint>(_internal)->SetHighLimit(_highLimit);
    }

    void CHingeJoint::SetLowLimit(Degree lowLimit)
    {
        if (_lowLimit == lowLimit)
            return;

        _lowLimit = lowLimit;

        if (_lowLimit > _highLimit)
            _lowLimit = _lowLimit;

        if (_internal != nullptr)
            std::static_pointer_cast<HingeJoint>(_internal)->SetLowLimit(_lowLimit);
    }

    void CHingeJoint::SetAngularOnly(bool angularOnly)
    {
        if (_angularOnly == angularOnly)
            return;

        _angularOnly = angularOnly;

        if (_internal != nullptr)
            std::static_pointer_cast<HingeJoint>(_internal)->SetAngularOnly(angularOnly);
    }

    void CHingeJoint::SetMotorEnabled(bool motorEnabled)
    {
        if (_motorEnabled == motorEnabled)
            return;

        _motorEnabled = motorEnabled;

        if (_internal != nullptr)
            std::static_pointer_cast<HingeJoint>(_internal)->SetMotorEnabled(motorEnabled);
    }

    void CHingeJoint::SetMaxMotorImpulse(float motorImpulse)
    {
        if (_motorImpulse == motorImpulse)
            return;

        _motorImpulse = motorImpulse;

        if (_internal != nullptr)
            std::static_pointer_cast<HingeJoint>(_internal)->SetMaxMotorImpulse(motorImpulse);
    }

    void CHingeJoint::SetMotorVelocity(float motorVelocity)
    {
        if (_motorVelocity == motorVelocity)
            return;

        _motorVelocity = motorVelocity;

        if (_internal != nullptr)
            std::static_pointer_cast<HingeJoint>(_internal)->SetMotorVelocity(motorVelocity);
    }
}
