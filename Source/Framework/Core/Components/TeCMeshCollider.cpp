#include "Components/TeCMeshCollider.h"
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
        SPtr<Collider> joint = MeshCollider::Create(*scene->GetPhysicsScene());

        return joint;
    }

    void CMeshCollider::Clone(const HMeshCollider& c)
    {
        CCollider::Clone(static_object_cast<CCollider>(c));
    }
}
