#include "Physics/TeMeshCollider.h"
#include "Physics/TePhysics.h"

namespace te
{
    MeshCollider::MeshCollider()
        : Collider(CoreType::TID_MeshCollider)
    { }

    SPtr<MeshCollider> MeshCollider::Create(PhysicsScene& scene, const Vector3& position, const Quaternion& rotation)
    {
        return scene.CreateMeshCollider(position, rotation);
    }
}
