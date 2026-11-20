#include "Components/TeCD6Joint.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CD6Joint::CD6Joint()
        : CJoint(CoreType::TID_CD6Joint)
    {
        SetName("D6Joint");
    }

    CD6Joint::CD6Joint(const HSceneObject& parent)
        : CJoint(parent, CoreType::TID_CD6Joint)
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

    bool CD6Joint::Clone(const HComponent& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        return Clone(static_object_cast<CD6Joint>(c), suffix);
    }

    bool CD6Joint::Clone(const HD6Joint& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        if (CJoint::Clone(static_object_cast<CJoint>(c), suffix))
        {
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

            return true;
        }

        return false;
    }

    void CD6Joint::OnEnabled()
    {
        CJoint::OnEnabled();

        if (_internal)
        {
            SPtr<D6Joint> joint = std::static_pointer_cast<D6Joint>(_internal);
            joint->SetLowerLinLimit(_lowerLinLimit);
            joint->SetUpperLinLimit(_upperLinLimit);
            joint->SetLowerAngLimit(_lowerAngLimit);
            joint->SetUpperAngLimit(_upperAngLimit);
            joint->SetLinearSpring(_linearSpring);
            joint->SetAngularSpring(_angularSpring);
            joint->SetLinearStiffness(_linearStiffness);
            joint->SetLinearDamping(_linearDamping);
            joint->SetAngularStiffness(_angularStiffness);
            joint->SetAngularDamping(_angularDamping);
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
            std::static_pointer_cast<D6Joint>(_internal)->SetUpperLinLimit(_upperLinLimit);
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
            std::static_pointer_cast<D6Joint>(_internal)->SetAngularSpring(angularSpring);
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
            std::static_pointer_cast<D6Joint>(_internal)->SetLinearDamping(linearDamping);
    }

    void CD6Joint::SetAngularDamping(float angularDamping)
    {
        if (_angularDamping == angularDamping)
            return;

        _angularDamping = angularDamping;

        if (_internal != nullptr)
            std::static_pointer_cast<D6Joint>(_internal)->SetAngularDamping(angularDamping);
    }

    void CD6Joint::ExportJson(nlohmann::json& document) const
    {
        CJoint::ExportJson(document);

        document["type"] = GetComponentType();

        document["lowerLinLimit"] = _lowerLinLimit;
        document["upperLinLimit"] = _upperLinLimit;
        document["lowerAngLimit"] = _lowerAngLimit.ValueDegrees();
        document["upperAngLimit"] = _upperAngLimit.ValueDegrees();
        document["linearSpring"] = _linearSpring;
        document["angularSpring"] = _angularSpring;
        document["linearStiffness"] = _linearStiffness;
        document["angularStiffness"] = _angularStiffness;
        document["linearDamping"] = _linearDamping;
        document["angularDamping"] = _angularDamping;
    }

    bool CD6Joint::ImportJson(const nlohmann::json& document, CD6Joint& joint)
    {
        CJoint::ImportJson(document, joint);

        if (document.contains("lowerLinLimit"))
            joint.SetLowerLinLimit(document["lowerLinLimit"].get<float>());
        if (document.contains("upperLinLimit"))
            joint.SetUpperLinLimit(document["upperLinLimit"].get<float>());
        if (document.contains("lowerAngLimit"))
            joint.SetLowerAngLimit(Degree(document["lowerAngLimit"].get<float>()));
        if (document.contains("upperAngLimit"))
            joint.SetUpperAngLimit(Degree(document["upperAngLimit"].get<float>()));
        if (document.contains("linearSpring"))
            joint.SetLinearSpring(document["linearSpring"].get<bool>());
        if (document.contains("angularSpring"))
            joint.SetAngularSpring(document["angularSpring"].get<bool>());
        if (document.contains("linearStiffness"))
            joint.SetLinearStiffness(document["linearStiffness"].get<float>());
        if (document.contains("angularStiffness"))
            joint.SetAngularStiffness(document["angularStiffness"].get<float>());
        if (document.contains("linearDamping"))
            joint.SetLinearDamping(document["linearDamping"].get<float>());
        if (document.contains("angularDamping"))
            joint.SetAngularDamping(document["angularDamping"].get<float>());

        return true;
    }
}
