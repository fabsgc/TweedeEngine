#include "Components/TeCSphereCollider.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CSphereCollider::CSphereCollider()
        : CCollider((UINT32)TID_CSphereCollider)
    {
        SetName("SphereCollider");
    }

    CSphereCollider::CSphereCollider(const HSceneObject& parent)
        : CCollider(parent, (UINT32)TID_CSphereCollider)
    {
        SetName("SphereCollider");
    }

    SPtr<Collider> CSphereCollider::CreateInternal()
    {
        const SPtr<SceneInstance>& scene = SO()->GetScene();
        SPtr<Collider> collider = SphereCollider::Create(*scene->GetPhysicsScene());
        collider->SetOwner(PhysicsOwnerType::Component);

        return collider;
    }

    void CSphereCollider::Clone(const HSphereCollider& c)
    {
        CCollider::Clone(static_object_cast<CCollider>(c));
    }
}
