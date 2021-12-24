#include "Components/TeCMeshSoftBody.h"

#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"

#include <functional>

using namespace std::placeholders;

namespace te
{
    CMeshSoftBody::CMeshSoftBody()
        : CSoftBody(HSceneObject(), (UINT32)TID_CMeshSoftBody)
    {
        SetName("MeshSoftBody");
    }

    CMeshSoftBody::CMeshSoftBody(const HSceneObject& parent)
        : CSoftBody(parent, (UINT32)TID_CMeshSoftBody)
    {
        SetName("MeshSoftBody");
    }

    SPtr<Body> CMeshSoftBody::CreateInternal()
    {
        SPtr<MeshSoftBody> body = MeshSoftBody::Create(SO());
        body->SetOwner(PhysicsOwnerType::Component, this);

        return body;
    }
}
