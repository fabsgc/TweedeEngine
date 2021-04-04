#include "Physics/TeCapsuleCollider.h"
#include "Physics/TePhysics.h"

namespace te
{
    SPtr<CapsuleCollider> CapsuleCollider::Create(PhysicsScene& scene)
    {
        return scene.CreateCapsuleCollider();
    }
}
