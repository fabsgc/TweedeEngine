#include "Physics/TeSphereCollider.h"
#include "Physics/TePhysics.h"

namespace te
{
    SphereCollider::SphereCollider()
        : Collider(CoreType::TID_SphereCollider)
    { }

    SPtr<SphereCollider> SphereCollider::Create(PhysicsScene& scene, float radius, const Vector3& position,
        const Quaternion& rotation)
    {
        return scene.CreateSphereCollider(radius, position, rotation);
    }
}
