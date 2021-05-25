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

    void CHingeJoint::Clone(const HHingeJoint& c)
    {
        CJoint::Clone(static_object_cast<CJoint>(c));

        _limitSoftness = c->_limitSoftness;
        _limitBias = c->_limitBias;
        _limitRelaxation = c->_limitRelaxation;
        _limitLow = c->_limitLow;
        _limitHigh = c->_limitHigh;
        _angularOnly = c->_angularOnly;
        _motorEnabled = c->_motorEnabled;
        _motorImpulse = c->_motorImpulse;
        _motorVelocity = c->_motorVelocity;
    }

    void CHingeJoint::OnEnabled()
    {
        CJoint::OnEnabled();

        if (_internal)
        {
            SetLimitSoftness(_limitSoftness);
            SetLimitBias(_limitBias);
            SetLimitRelaxation(_limitRelaxation);
            SetLimitHigh(_limitHigh);
            SetLimitLow(_limitLow);
            SetAngularOnly(_angularOnly);
            SetMotorEnabled(_motorEnabled);
            SetMaxMotorImpulse(_motorImpulse);
            SetMotorVelocity(_motorVelocity);
        }
    }

    void CHingeJoint::SetLimitSoftness(float softness)
    {
        if (_limitSoftness == softness)
            return;

        _limitSoftness = Math::Clamp(softness, 0.0f, 1.0f);

        if (_internal != nullptr)
            std::static_pointer_cast<HingeJoint>(_internal)->SetLimitSoftness(softness);
    }

    void CHingeJoint::SetLimitBias(float bias)
    {
        if (_limitBias == bias)
            return;

        _limitBias = Math::Clamp(bias, 0.0f, 1.0f);

        if (_internal != nullptr)
            std::static_pointer_cast<HingeJoint>(_internal)->SetLimitBias(bias);
    }

    void CHingeJoint::SetLimitRelaxation(float relaxation)
    {
        if (_limitRelaxation == relaxation)
            return;

        _limitRelaxation = Math::Clamp(relaxation, 0.0f, 1.0f);

        if (_internal != nullptr)
            std::static_pointer_cast<HingeJoint>(_internal)->SetLimitRelaxation(relaxation);
    }

    void CHingeJoint::SetLimitHigh(Degree limitHigh)
    {
        if (_limitHigh == limitHigh)
            return;

        _limitHigh = limitHigh;

        if (_limitHigh < _limitLow)
            _limitLow = _limitLow;

        if (_internal != nullptr)
            std::static_pointer_cast<HingeJoint>(_internal)->SetLimitHigh(_limitHigh);
    }

    void CHingeJoint::SetLimitLow(Degree limitLow)
    {
        if (_limitLow == limitLow)
            return;

        _limitLow = limitLow;

        if (_limitLow > _limitHigh)
            _limitLow = _limitLow;

        if (_internal != nullptr)
            std::static_pointer_cast<HingeJoint>(_internal)->SetLimitLow(_limitLow);
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
