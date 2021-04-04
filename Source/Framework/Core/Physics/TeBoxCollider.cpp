#include "Physics/TeBoxCollider.h"
#include "Physics/TePhysics.h"

namespace te
{
    SPtr<BoxCollider> BoxCollider::Create(PhysicsScene& scene)
    {
        return scene.CreateBoxCollider();
    }
}
