#include "Components/TeCCapsuleCollider.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CCapsuleCollider::CCapsuleCollider()
        : CCollider((UINT32)TID_CCapsuleCollider)
    {
        SetName("CapsuleCollider");
    }

    CCapsuleCollider::CCapsuleCollider(const HSceneObject& parent)
        : CCollider(parent, (UINT32)TID_CCapsuleCollider)
    {
        SetName("CapsuleCollider");
    }

    SPtr<Collider> CCapsuleCollider::CreateInternal()
    {
        const SPtr<SceneInstance>& scene = SO()->GetScene();
        SPtr<Collider> joint = CapsuleCollider::Create(*scene->GetPhysicsScene());

        return joint;
    }

    void CCapsuleCollider::Clone(const HCapsuleCollider& c)
    {
        CCollider::Clone(static_object_cast<CCollider>(c));
    }
}
