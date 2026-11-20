#include "Components/TeCRopeSoftBody.h"

#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"

namespace te
{
    CRopeSoftBody::CRopeSoftBody()
        : CSoftBody(HSceneObject(), CoreType::TID_CRopeSoftBody)
    {
        SetName("RopeSoftBody");
    }

    CRopeSoftBody::CRopeSoftBody(const HSceneObject& parent)
        : CSoftBody(parent, CoreType::TID_CRopeSoftBody)
    {
        SetName("RopeSoftBody");
    }

    bool CRopeSoftBody::Clone(const HComponent& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        return Clone(static_object_cast<CRopeSoftBody>(c), suffix);
    }

    bool CRopeSoftBody::Clone(const HRopeSoftBody& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        if (CSoftBody::Clone(static_object_cast<CSoftBody>(c), suffix))
        {
            _from = c->_from;
            _to = c->_to;
            _resolution = c->_resolution;

            return true;
        }

        return false;
    }

    void CRopeSoftBody::SetFrom(const Vector3& from)
    {
        if (_from == from)
            return;

        _from = from;

        if (_internal != nullptr)
            std::static_pointer_cast<RopeSoftBody>(_internal)->SetFrom(from);
    }

    void CRopeSoftBody::SetTo(const Vector3& to)
    {
        if (_to == to)
            return;

        _to = to;

        if (_internal != nullptr)
            std::static_pointer_cast<RopeSoftBody>(_internal)->SetTo(to);
    }

    void CRopeSoftBody::SetResolution(UINT32 resolution)
    {
        if (_resolution == resolution)
            return;

        _resolution = resolution;

        if (_internal != nullptr)
            std::static_pointer_cast<RopeSoftBody>(_internal)->SetResolution(resolution);
    }

    void CRopeSoftBody::OnEnabled()
    {
        CSoftBody::OnEnabled();

        if (_internal)
        { 
            std::static_pointer_cast<RopeSoftBody>(_internal)->SetFrom(_from);
            std::static_pointer_cast<RopeSoftBody>(_internal)->SetTo(_to);
            std::static_pointer_cast<RopeSoftBody>(_internal)->SetResolution(_resolution);
        }
    }

    SPtr<Body> CRopeSoftBody::CreateInternal()
    {
        SPtr<RopeSoftBody> body = RopeSoftBody::Create(SO());
        body->SetOwner(PhysicsOwnerType::Component, this);

        return body;
    }

    void CRopeSoftBody::ExportJson(nlohmann::json& document) const
    {
        CSoftBody::ExportJson(document);

        document["type"] = GetComponentType();
        _from.ExportJson(document["from"]);
        _to.ExportJson(document["to"]);
        document["resolution"] = _resolution;
    }

    bool CRopeSoftBody::ImportJson(const nlohmann::json& document, CRopeSoftBody& softBody)
    {
        CSoftBody::ImportJson(document, softBody);

        if (document.contains("from"))
            softBody.SetFrom(Vector3::ImportJson(document["from"]));
        if (document.contains("to"))
            softBody.SetTo(Vector3::ImportJson(document["to"]));
        if (document.contains("resolution"))
            softBody.SetResolution(document["resolution"].get<UINT32>());

        return true;
    }
}
