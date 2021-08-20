#include "Physics/TeCollider.h"
#include "Physics/TePhysics.h"
#include "Physics/TeBody.h"
#include "Physics/TeFCollider.h"
#include "Math/TeRay.h"

namespace te
{
    void Collider::SetBody(Body* value)
    {
        _body = value;
    }

    void Collider::SetScale(const Vector3& scale)
    {
        _internal->SetScale(scale);
    }

    Vector3 Collider::GetScale() const
    {
        return _internal->GetScale();
    }

    void Collider::SetCenter(const Vector3& center)
    {
        _internal->SetCenter(center);
    }

    const Vector3& Collider::GetCenter() const
    {
        return _internal->GetCenter();
    }

    void Collider::SetPosition(const Vector3& position)
    {
        _internal->SetPosition(position);
    }

    const Vector3& Collider::GetPosition() const
    {
        return _internal->GetPosition();
    }

    void Collider::SetRotation(const Quaternion& rotation)
    {
        _internal->SetRotation(rotation);
    }

    const Quaternion& Collider::GetRotation() const
    {
        return _internal->GetRotation();
    }

    bool Collider::RayCast(const SPtr<PhysicsScene>& physicsScene, const Ray& ray, PhysicsQueryHit& hit, float maxDist) const
    {
        return RayCast(physicsScene, ray.GetOrigin(), ray.GetDirection(), hit, maxDist);
    }

    bool Collider::RayCast(const SPtr<PhysicsScene>& physicsScene, const Vector3& origin, const Vector3& unitDir, PhysicsQueryHit& hit, float maxDist) const
    {
        bool somethingHit = physicsScene->RayCast(origin, unitDir, hit, maxDist);

        if (somethingHit && hit.HitColliderRaw == this)
            return true;

        return false;
    }

    bool Collider::RayCast(const SPtr<PhysicsScene>& physicsScene, const Ray& ray, Vector<PhysicsQueryHit>& hits, float maxDist) const
    {
        return RayCast(physicsScene, ray.GetOrigin(), ray.GetDirection(), hits, maxDist);
    }

    bool Collider::RayCast(const SPtr<PhysicsScene>& physicsScene, const Vector3& origin, const Vector3& unitDir, Vector<PhysicsQueryHit>& hits, float maxDist) const
    {
        bool somethingHit = physicsScene->RayCast(origin, unitDir, hits, maxDist);

        if (somethingHit)
        {
            for (auto& hit : hits)
            {
                if (hit.HitColliderRaw == this)
                    return true;
            }
        }

        return false;
    }
}
