#include "Components/TeCMeshCollider.h"
#include "Components/TeCBody.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CMeshCollider::CMeshCollider()
        : CCollider((UINT32)TID_CMeshCollider)
    {
        SetName("MeshCollider");
    }

    CMeshCollider::CMeshCollider(const HSceneObject& parent)
        : CCollider(parent, (UINT32)TID_CMeshCollider)
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
    }

    bool CMeshCollider::IsValidParent(const HBody& parent) const
    {
        return _mesh.IsLoaded();
    }

    void CMeshCollider::Clone(const HMeshCollider& c)
    {
        CCollider::Clone(static_object_cast<CCollider>(c));

        _mesh = c->_mesh;
        _collisionType = c->_collisionType;
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

            _getInternal()->SetCollisionType(type);

            if (_parent != nullptr)
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

            UpdateParentBody();
            _getInternal()->SetMesh(mesh);

            if (_parent != nullptr)
                _parent->AddCollider(static_object_cast<CCollider>(GetHandle()));
        }
    }
}
