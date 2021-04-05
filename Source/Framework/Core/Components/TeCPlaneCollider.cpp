#include "Components/TeCPlaneCollider.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CPlaneCollider::CPlaneCollider()
        : CCollider((UINT32)TID_CPlaneCollider)
    {
        SetName("PlaneCollider");
    }

    CPlaneCollider::CPlaneCollider(const HSceneObject& parent)
        : CCollider(parent, (UINT32)TID_CPlaneCollider)
    {
        SetName("PlaneCollider");
    }

    SPtr<Collider> CPlaneCollider::CreateInternal()
    {
        const SPtr<SceneInstance>& scene = SO()->GetScene();
        SPtr<Collider> collider = PlaneCollider::Create(*scene->GetPhysicsScene());
        collider->SetOwner(PhysicsOwnerType::Component);

        return collider;
    }

    void CPlaneCollider::Clone(const HPlaneCollider& c)
    {
        CCollider::Clone(static_object_cast<CCollider>(c));
    }
}
