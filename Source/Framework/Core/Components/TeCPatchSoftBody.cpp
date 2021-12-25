#include "Components/TeCPatchSoftBody.h"

#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"

namespace te
{
    CPatchSoftBody::CPatchSoftBody()
        : CSoftBody(HSceneObject(), (UINT32)TID_CPatchSoftBody)
    {
        SetName("PatchSoftBody");
    }

    CPatchSoftBody::CPatchSoftBody(const HSceneObject& parent)
        : CSoftBody(parent, (UINT32)TID_CPatchSoftBody)
    {
        SetName("PatchSoftBody");
    }

    bool CPatchSoftBody::Clone(const HComponent& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        return Clone(static_object_cast<CPatchSoftBody>(c), suffix);
    }

    bool CPatchSoftBody::Clone(const HPatchSoftBody& c, const String& suffix)
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

    void CPatchSoftBody::OnEnabled()
    {
        CSoftBody::OnEnabled();

        if (_internal)
        { } // TODO
    }

    SPtr<Body> CPatchSoftBody::CreateInternal()
    {
        SPtr<PatchSoftBody> body = PatchSoftBody::Create(SO());
        body->SetOwner(PhysicsOwnerType::Component, this);

        return body;
    }
}
