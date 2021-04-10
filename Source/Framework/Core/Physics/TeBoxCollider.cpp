#include "Physics/TeBoxCollider.h"
#include "Physics/TePhysics.h"

namespace te
{
    SPtr<BoxCollider> BoxCollider::Create(PhysicsScene& scene, const Vector3& extents,
        const Vector3& position, const Quaternion& rotation)
    {
        return scene.CreateBoxCollider(extents, position, rotation);
    }
}
