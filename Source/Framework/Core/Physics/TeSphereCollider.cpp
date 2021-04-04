#include "Physics/TeSphereCollider.h"
#include "Physics/TePhysics.h"

namespace te
{
    SPtr<SphereCollider> SphereCollider::Create(PhysicsScene& scene)
    {
        return scene.CreateSphereCollider();
    }
}
