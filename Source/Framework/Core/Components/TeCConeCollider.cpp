#include "Components/TeCConeCollider.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CConeCollider::CConeCollider()
        : CCollider((UINT32)TID_CConeCollider)
    {
        SetName("ConeCollider");
    }

    CConeCollider::CConeCollider(const HSceneObject& parent)
        : CCollider(parent, (UINT32)TID_CConeCollider)
    {
        SetName("ConeCollider");
    }

    SPtr<Collider> CConeCollider::CreateInternal()
    {
        const SPtr<SceneInstance>& scene = SO()->GetScene();
        SPtr<Collider> collider = ConeCollider::Create(*scene->GetPhysicsScene());
        collider->SetOwner(PhysicsOwnerType::Component);

        return collider;
    }

    void CConeCollider::Clone(const HConeCollider& c)
    {
        CCollider::Clone(static_object_cast<CCollider>(c));
    }
}
