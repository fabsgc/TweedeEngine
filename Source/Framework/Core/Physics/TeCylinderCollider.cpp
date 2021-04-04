#include "Physics/TeCylinderCollider.h"
#include "Physics/TePhysics.h"

namespace te
{
    SPtr<CylinderCollider> CylinderCollider::Create(PhysicsScene& scene)
    {
        return scene.CreateCylinderCollider();
    }
}
