#include "Physics/TeCapsuleCollider.h"
#include "Physics/TePhysics.h"

namespace te
{
    SPtr<CapsuleCollider> CapsuleCollider::Create(PhysicsScene& scene, float radius, float height,
        const Vector3& position, const Quaternion& rotation)
    {
        return scene.CreateCapsuleCollider(radius, height, position, rotation);
    }
}
