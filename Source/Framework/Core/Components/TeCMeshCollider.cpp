#include "Components/TeCMeshCollider.h"
#include "Components/TeCRigidBody.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"
#include "Physics/TePhysicsMesh.h"
#include "Serialization/TeUtility.h"
#include "Resources/TeResourceManager.h"

namespace te
{
    CMeshCollider::CMeshCollider()
        : CCollider(CoreType::TID_CMeshCollider)
    {
        SetName("MeshCollider");
    }

    CMeshCollider::CMeshCollider(const HSceneObject& parent)
        : CCollider(parent, CoreType::TID_CMeshCollider)
    {
        SetName("MeshCollider");
    }

    SPtr<Collider> CMeshCollider::CreateInternal()
    {
        const SPtr<SceneInstance>& scene = SO()->GetScene();

        SPtr<MeshCollider> collider = MeshCollider::Create(*scene->GetPhysicsScene());
        collider->SetMesh(_mesh);
        collider->SetOwner(PhysicsOwnerType::Component, this);

        return collider;
    }

    void CMeshCollider::RestoreInternal()
    {
        CCollider::RestoreInternal();

        SetCollisionType(_collisionType);
        SetMesh(_mesh);
    }

    bool CMeshCollider::IsValidParent(const HRigidBody& parent) const
    {
        return _mesh.IsLoaded();
    }

    bool CMeshCollider::Clone(const HComponent& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        return Clone(static_object_cast<CMeshCollider>(c), suffix);
    }

    bool CMeshCollider::Clone(const HMeshCollider& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        if (CCollider::Clone(static_object_cast<CCollider>(c), suffix))
        {
            _mesh = c->_mesh;
            _collisionType = c->_collisionType;

            return true;
        }

        return false;
    }

    void CMeshCollider::SetCollisionType(PhysicsMeshType type)
    {
        if (_collisionType == type)
            return;

        _collisionType = type;

        if (_internal != nullptr)
        {
            if (_parent != nullptr)
                _parent->RemoveCollider(static_object_cast<CCollider>(GetHandle()));

            GetInternal()->SetCollisionType(type);

            if (_parent.Empty() || !_mesh.IsLoaded())
                UpdateParentBody();
            else
                _parent->AddCollider(static_object_cast<CCollider>(GetHandle()));   
        }
    }

    void CMeshCollider::SetMesh(const HPhysicsMesh& mesh)
    {
        if (_mesh == mesh)
            return;

        _mesh = mesh;

        if (_internal != nullptr)
        {
            if (_parent != nullptr)
                _parent->RemoveCollider(static_object_cast<CCollider>(GetHandle()));

            GetInternal()->SetMesh(mesh);

            if (_parent.Empty() || !_mesh.IsLoaded())
                UpdateParentBody();
            else
                _parent->AddCollider(static_object_cast<CCollider>(GetHandle()));
        }
    }

    void CMeshCollider::ExportJson(nlohmann::json& document) const
    {
        CCollider::ExportJson(document);

        document["type"] = GetComponentType();

        document["mesh"] = _mesh.IsLoaded() ? serialization::GetResourceName(_mesh.Get()) : "";
        document["collisionType"] = static_cast<uint32_t>(_collisionType);
    }

    bool CMeshCollider::ImportJson(const nlohmann::json& document, CMeshCollider& collider)
    {
        Component::ImportJson(document, collider);

        if (document.contains("mesh"))
        {
            HPhysicsMesh mesh = static_resource_cast<PhysicsMesh>(
                gResourceManager().Get(serialization::GetResourceUUID(document["mesh"].get<String>())));
            collider.SetMesh(mesh);
        }

        if (document.contains("collisionType"))
            collider.SetCollisionType(static_cast<PhysicsMeshType>(document["collisionType"].get<uint32_t>()));

        return true;
    }
}
