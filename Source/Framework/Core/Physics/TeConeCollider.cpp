#include "Physics/TeConeCollider.h"
#include "Physics/TePhysics.h"

namespace te
{
    SPtr<ConeCollider> ConeCollider::Create(PhysicsScene& scene)
    {
        return scene.CreateConeCollider();
    }
}
