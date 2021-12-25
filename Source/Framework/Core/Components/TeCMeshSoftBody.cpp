#include "Components/TeCMeshSoftBody.h"

#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"

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

    bool CMeshSoftBody::Clone(const HComponent& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        return Clone(static_object_cast<CMeshSoftBody>(c), suffix);
    }

    bool CMeshSoftBody::Clone(const HMeshSoftBody& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        if (CSoftBody::Clone(static_object_cast<CSoftBody>(c), suffix))
        {
            _mesh = c->_mesh;

            return true;
        }

        return false;
    }

    void CMeshSoftBody::SetMesh(const HPhysicsMesh& mesh)
    {
        if (_mesh == mesh)
            return;

        _mesh = mesh;

        if (_internal != nullptr)
            std::static_pointer_cast<MeshSoftBody>(_internal)->SetMesh(mesh);
    }

    void CMeshSoftBody::OnEnabled()
    {
        CSoftBody::OnEnabled();

        if (_internal)
            std::static_pointer_cast<MeshSoftBody>(_internal)->SetMesh(_mesh);
    }

    SPtr<Body> CMeshSoftBody::CreateInternal()
    {
        SPtr<MeshSoftBody> body = MeshSoftBody::Create(SO());
        body->SetOwner(PhysicsOwnerType::Component, this);

        return body;
    }
}
