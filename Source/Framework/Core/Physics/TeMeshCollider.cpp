#include "Physics/TeMeshCollider.h"
#include "Physics/TePhysics.h"

namespace te
{
    SPtr<MeshCollider> MeshCollider::Create(PhysicsScene& scene)
    {
        return scene.CreateMeshCollider();
    }
}
