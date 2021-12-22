#include "TeBody.h"
#include "Math/TeRay.h"
#include "Physics/TePhysics.h"
#include "Scene/TeSceneObject.h"

namespace te
{ 
    Body::Body(const HSceneObject& linkedSO, UINT32 type)
        : Serializable(type)
        , _flags(BodyFlag::None)
        , _linkedSO(linkedSO)
    { 
        TE_ASSERT_ERROR(linkedSO.Empty(), "You try to link a body to a SceneObject using an invalid handle");
    }

    bool Body::RayCast(const SPtr<PhysicsScene>& physicsScene, const Ray& ray, PhysicsQueryHit& hit, float maxDist) const
    {
        return RayCast(physicsScene, ray.GetOrigin(), ray.GetDirection(), hit, maxDist);
    }

    bool Body::RayCast(const SPtr<PhysicsScene>& physicsScene, const Vector3& origin, const Vector3& unitDir, PhysicsQueryHit& hit, float maxDist) const
    {
        bool somethingHit = physicsScene->RayCast(origin, unitDir, hit, maxDist);
        if (somethingHit && hit.HitBodyRaw == this)
            return true;

        return false;
    }

    bool Body::RayCast(const SPtr<PhysicsScene>& physicsScene, const Ray& ray, Vector<PhysicsQueryHit>& hits, float maxDist) const
    {
        return RayCast(physicsScene, ray.GetOrigin(), ray.GetDirection(), hits, maxDist);
    }

    bool Body::RayCast(const SPtr<PhysicsScene>& physicsScene, const Vector3& origin, const Vector3& unitDir, Vector<PhysicsQueryHit>& hits, float maxDist) const
    {
        bool somethingHit = physicsScene->RayCast(origin, unitDir, hits, maxDist);
        if (somethingHit)
        {
            for (auto& hit : hits)
            {
                if (hit.HitBodyRaw == this)
                    return true;
            }
        }

        return false;
    }

    void Body::_setTransform(const Vector3& position, const Quaternion& rotation)
    {
        _linkedSO->SetWorldPosition(position);
        _linkedSO->SetWorldRotation(rotation);
    }

    void Body::SetLinkedSO(const HSceneObject& SO)
    {
        TE_ASSERT_ERROR(SO.Empty(), "You try to link a body to a SceneObject using an invalid handle");
        _linkedSO = SO;
    }
}
