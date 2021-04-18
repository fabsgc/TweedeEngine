#include "Physics/TeHeightFieldCollider.h"
#include "Physics/TePhysics.h"

namespace te
{
    SPtr<HeightFieldCollider> HeightFieldCollider::Create(PhysicsScene& scene, const Vector3& position, const Quaternion& rotation)
    {
        return scene.CreateHeightFieldCollider(position, rotation);
    }
}
