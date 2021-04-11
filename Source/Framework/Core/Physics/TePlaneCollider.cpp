#include "Physics/TePlaneCollider.h"
#include "Physics/TePhysics.h"

namespace te
{
    SPtr<PlaneCollider> PlaneCollider::Create(PhysicsScene& scene, const Vector3& normal,
        const Vector3& position, const Quaternion& rotation)
    {
        return scene.CreatePlaneCollider(normal, position, rotation);
    }
}
