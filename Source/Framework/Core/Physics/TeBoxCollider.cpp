#include "Physics/TeBoxCollider.h"
#include "Physics/TePhysics.h"

namespace te
{
    BoxCollider::BoxCollider()
        : Collider(CoreType::TID_BoxCollider)
    { }

    SPtr<BoxCollider> BoxCollider::Create(PhysicsScene& scene, const Vector3& extents,
        const Vector3& position, const Quaternion& rotation)
    {
        return scene.CreateBoxCollider(extents, position, rotation);
    }
}
