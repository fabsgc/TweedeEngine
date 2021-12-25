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
            // TODO
            return true;
        }

        return false;
    }

    void CRopeSoftBody::OnEnabled()
    {
        CSoftBody::OnEnabled();

        if (_internal)
        { } // TODO
    }

    SPtr<Body> CRopeSoftBody::CreateInternal()
    {
        SPtr<RopeSoftBody> body = RopeSoftBody::Create(SO());
        body->SetOwner(PhysicsOwnerType::Component, this);

        return body;
    }
}
