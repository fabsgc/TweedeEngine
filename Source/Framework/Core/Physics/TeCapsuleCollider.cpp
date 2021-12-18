#include "Physics/TeCapsuleCollider.h"
#include "Physics/TePhysics.h"

namespace te
{
    CapsuleCollider::CapsuleCollider()
        : Collider(TypeID_Core::TID_CapsuleCollider)
    { }

    SPtr<CapsuleCollider> CapsuleCollider::Create(PhysicsScene& scene, float radius, float height,
        const Vector3& position, const Quaternion& rotation)
    {
        return scene.CreateCapsuleCollider(radius, height, position, rotation);
    }
}
