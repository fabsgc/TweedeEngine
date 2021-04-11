#include "Physics/TeCylinderCollider.h"
#include "Physics/TePhysics.h"

namespace te
{
    SPtr<CylinderCollider> CylinderCollider::Create(PhysicsScene& scene, const Vector3& extents,
        const Vector3& position, const Quaternion& rotation)
    {
        return scene.CreateCylinderCollider(extents, position, rotation);
    }
}
