#include "Components/TeCCylinderCollider.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CCylinderCollider::CCylinderCollider()
        : CCollider((UINT32)TID_CCylinderCollider)
    {
        SetName("CylinderCollider");
    }

    CCylinderCollider::CCylinderCollider(const HSceneObject& parent)
        : CCollider(parent, (UINT32)TID_CCylinderCollider)
    {
        SetName("CylinderCollider");
    }

    SPtr<Collider> CCylinderCollider::CreateInternal()
    {
        const SPtr<SceneInstance>& scene = SO()->GetScene();
        SPtr<Collider> collider = CylinderCollider::Create(*scene->GetPhysicsScene());
        collider->SetOwner(PhysicsOwnerType::Component, this);

        return collider;
    }

    void CCylinderCollider::Clone(const HCylinderCollider& c)
    {
        CCollider::Clone(static_object_cast<CCollider>(c));
    }
}
