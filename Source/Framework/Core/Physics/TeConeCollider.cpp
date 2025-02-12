#include "Physics/TeConeCollider.h"
#include "Physics/TePhysics.h"

namespace te
{
    ConeCollider::ConeCollider()
        : Collider(CoreType::TID_ConeCollider)
    { }

    SPtr<ConeCollider> ConeCollider::Create(PhysicsScene& scene, float radius, float height,
        const Vector3& position, const Quaternion& rotation)
    {
        return scene.CreateConeCollider(radius, height, position, rotation);
    }
}
