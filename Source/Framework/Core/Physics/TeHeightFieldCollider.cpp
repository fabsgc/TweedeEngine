#include "Physics/TeHeightFieldCollider.h"
#include "Physics/TePhysics.h"

namespace te
{
    HeightFieldCollider::HeightFieldCollider()
        : Collider(TypeID_Core::TID_HeightFieldCollider)
    { }

    SPtr<HeightFieldCollider> HeightFieldCollider::Create(PhysicsScene& scene, const Vector3& position, const Quaternion& rotation)
    {
        return scene.CreateHeightFieldCollider(position, rotation);
    }
}
