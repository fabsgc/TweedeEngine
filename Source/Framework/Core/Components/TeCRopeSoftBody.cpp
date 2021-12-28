#include "Components/TeCRopeSoftBody.h"

#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"

namespace te
{
    CRopeSoftBody::CRopeSoftBody()
        : CSoftBody(HSceneObject(), (UINT32)TID_CRopeSoftBody)
    {
        SetName("RopeSoftBody");
    }

    CRopeSoftBody::CRopeSoftBody(const HSceneObject& parent)
        : CSoftBody(parent, (UINT32)TID_CRopeSoftBody)
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

        std::static_pointer_cast<RopeSoftBody>(_internal)->SetFrom(from);
    }

    void CRopeSoftBody::SetTo(const Vector3& to)
    {
        if (_to == to)
            return;

        _to = to;

        std::static_pointer_cast<RopeSoftBody>(_internal)->SetTo(to);
    }

    void CRopeSoftBody::SetResolution(UINT32 resolution)
    {
        if (_resolution == resolution)
            return;

        _resolution = resolution;

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
}
