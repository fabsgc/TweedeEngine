#include "Physics/TePlaneCollider.h"
#include "Physics/TePhysics.h"

namespace te
{
    SPtr<PlaneCollider> PlaneCollider::Create(PhysicsScene& scene)
    {
        return scene.CreatePlaneCollider();
    }
}
