#include "Components/TeCEllipsoidSoftBody.h"

#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"

namespace te
{
    CEllipsoidSoftBody::CEllipsoidSoftBody()
        : CSoftBody(HSceneObject(), (UINT32)TID_CEllipsoidSoftBody)
    {
        SetName("EllipsoidSoftBody");
    }

    CEllipsoidSoftBody::CEllipsoidSoftBody(const HSceneObject& parent)
        : CSoftBody(parent, (UINT32)TID_CEllipsoidSoftBody)
    {
        SetName("EllipsoidSoftBody");
    }

    bool CEllipsoidSoftBody::Clone(const HComponent& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        return Clone(static_object_cast<CEllipsoidSoftBody>(c), suffix);
    }

    bool CEllipsoidSoftBody::Clone(const HEllipsoidSoftBody& c, const String& suffix)
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

    void CEllipsoidSoftBody::OnEnabled()
    {
        CSoftBody::OnEnabled();

        if (_internal)
        { } // TODO
    }

    SPtr<Body> CEllipsoidSoftBody::CreateInternal()
    {
        SPtr<EllipsoidSoftBody> body = EllipsoidSoftBody::Create(SO());
        body->SetOwner(PhysicsOwnerType::Component, this);

        return body;
    }
}
