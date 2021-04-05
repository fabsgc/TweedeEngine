#include "Components/TeCBoxCollider.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CBoxCollider::CBoxCollider()
        : CCollider((UINT32)TID_CBoxCollider)
    {
        SetName("BoxCollider");
    }

    CBoxCollider::CBoxCollider(const HSceneObject& parent)
        : CCollider(parent, (UINT32)TID_CBoxCollider)
    {
        SetName("BoxCollider");
    }

    SPtr<Collider> CBoxCollider::CreateInternal()
    {
        const SPtr<SceneInstance>& scene = SO()->GetScene();
        SPtr<Collider> collider = BoxCollider::Create(*scene->GetPhysicsScene());
        collider->SetOwner(PhysicsOwnerType::Component);

        return collider;
    }

    void CBoxCollider::Clone(const HBoxCollider& c)
    {
        CCollider::Clone(static_object_cast<CCollider>(c));
    }
}
