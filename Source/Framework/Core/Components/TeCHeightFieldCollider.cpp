#include "Components/TeCHeightFieldCollider.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CHeightFieldCollider::CHeightFieldCollider()
        : CCollider((UINT32)TID_CHeightFieldCollider)
    {
        SetName("HeightFieldCollider");
    }

    CHeightFieldCollider::CHeightFieldCollider(const HSceneObject& parent)
        : CCollider(parent, (UINT32)TID_CHeightFieldCollider)
    {
        SetName("HeightFieldCollider");
    }

    SPtr<Collider> CHeightFieldCollider::CreateInternal()
    {
        const SPtr<SceneInstance>& scene = SO()->GetScene();

        SPtr<Collider> collider = HeightFieldCollider::Create(*scene->GetPhysicsScene());
        collider->SetOwner(PhysicsOwnerType::Component, this);

        return collider;
    }

    void CHeightFieldCollider::Clone(const HHeightFieldCollider& c)
    {
        CCollider::Clone(static_object_cast<CCollider>(c));
    }
}
