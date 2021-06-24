#include "Components/TeCD6Joint.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CD6Joint::CD6Joint()
        : CJoint((UINT32)TID_CD6Joint)
    {
        SetName("D6Joint");
    }

    CD6Joint::CD6Joint(const HSceneObject& parent)
        : CJoint(parent, (UINT32)TID_CD6Joint)
    {
        SetName("D6Joint");
    }

    SPtr<Joint> CD6Joint::CreateInternal()
    {
        const SPtr<SceneInstance>& scene = SO()->GetScene();
        SPtr<Joint> joint = D6Joint::Create(*scene->GetPhysicsScene());
        joint->SetOwner(PhysicsOwnerType::Component, this);

        return joint;
    }

    void CD6Joint::Clone(const HD6Joint& c)
    {
        CJoint::Clone(static_object_cast<CJoint>(c));

        _lowerLinLimit = c->_lowerLinLimit;
        _upperLinLimit = c->_upperLinLimit;
        _lowerAngLimit = c->_lowerAngLimit;
        _upperAngLimit = c->_upperAngLimit;

        _linearSpring = c->_linearSpring;
        _angularSpring = c->_angularSpring;
        _linearStiffness = c->_linearStiffness;
        _linearDamping = c->_linearDamping;
        _angularStiffness = c->_angularStiffness;
        _angularDamping = c->_angularDamping;
    }

    void CD6Joint::OnEnabled()
    {
        CJoint::OnEnabled();

        if (_internal)
        {
            SetLowerLinLimit(_lowerLinLimit);
            SetUpperLinLimit(_upperLinLimit);
            SetLowerAngLimit(_lowerAngLimit);
            SetUpperAngLimit(_upperAngLimit);

            SetLinearSpring(_linearSpring);
            SetAngularSpring(_angularSpring);
            SetLinearStiffness(_linearStiffness);
            SetLinearDamping(_linearDamping);
            SetAngularStiffness(_angularStiffness);
            SetAngularDamping(_angularDamping);
        }
    }

    void CD6Joint::SetLowerLinLimit(float lowerLinLimit)
    {
        if (_lowerLinLimit == lowerLinLimit)
            return;

        _lowerLinLimit = Math::Clamp(lowerLinLimit, 0.0f, 1.0f);

        if (_internal != nullptr)
            std::static_pointer_cast<D6Joint>(_internal)->SetLowerLinLimit(_lowerLinLimit);
    }

    void CD6Joint::SetUpperLinLimit(float upperLinLimit)
    {
        if (_upperLinLimit == upperLinLimit)
            return;

        _upperLinLimit = Math::Clamp(upperLinLimit, 0.0f, 1.0f);

        if (_internal != nullptr)
            std::static_pointer_cast<D6Joint>(_internal)->SetLowerLinLimit(_upperLinLimit);
    }

    void CD6Joint::SetLowerAngLimit(Degree lowerAngLimit)
    {
        if (_lowerAngLimit == lowerAngLimit)
            return;

        _lowerAngLimit = Math::Clamp(lowerAngLimit, Degree(0), Degree(90));

        if (_internal != nullptr)
            std::static_pointer_cast<D6Joint>(_internal)->SetLowerAngLimit(_lowerAngLimit);
    }

    void CD6Joint::SetUpperAngLimit(Degree upperAngLimit)
    {
        if (_upperAngLimit == upperAngLimit)
            return;

        _upperAngLimit = Math::Clamp(upperAngLimit, Degree(0), Degree(90));

        if (_internal != nullptr)
            std::static_pointer_cast<D6Joint>(_internal)->SetUpperAngLimit(_upperAngLimit);
    }

    void CD6Joint::SetLinearSpring(bool linearSpring)
    {
        if (_linearSpring == linearSpring)
            return;

        _linearSpring = linearSpring;

        if (_internal != nullptr)
            std::static_pointer_cast<D6Joint>(_internal)->SetLinearSpring(linearSpring);
    }

    void CD6Joint::SetAngularSpring(bool angularSpring)
    {
        if (_angularSpring == angularSpring)
            return;

        _angularSpring = angularSpring;

        if (_internal != nullptr)
            std::static_pointer_cast<D6Joint>(_internal)->SetLinearSpring(angularSpring);
    }

    void CD6Joint::SetLinearStiffness(float linearStiffness)
    {
        if (_linearStiffness == linearStiffness)
            return;

        _linearStiffness = linearStiffness;

        if (_internal != nullptr)
            std::static_pointer_cast<D6Joint>(_internal)->SetLinearStiffness(linearStiffness);
    }

    void CD6Joint::SetAngularStiffness(float angularStiffness)
    {
        if (_angularStiffness == angularStiffness)
            return;

        _angularStiffness = angularStiffness;

        if (_internal != nullptr)
            std::static_pointer_cast<D6Joint>(_internal)->SetAngularStiffness(angularStiffness);
    }

    void CD6Joint::SetLinearDamping(float linearDamping)
    {
        if (_linearDamping == linearDamping)
            return;

        _linearDamping = linearDamping;

        if (_internal != nullptr)
            std::static_pointer_cast<D6Joint>(_internal)->SetAngularStiffness(linearDamping);
    }

    void CD6Joint::SetAngularDamping(float angularDamping)
    {
        if (_angularDamping == angularDamping)
            return;

        _angularDamping = angularDamping;

        if (_internal != nullptr)
            std::static_pointer_cast<D6Joint>(_internal)->SetAngularDamping(angularDamping);
    }
}
