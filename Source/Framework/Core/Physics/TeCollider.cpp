#include "Physics/TeCollider.h"
#include "Physics/TePhysics.h"
#include "Physics/TeBody.h"
#include "Physics/TeFCollider.h"

namespace te
{
    void Collider::SetIsTrigger(bool value)
    {
        _internal->SetIsTrigger(value);
    }

    bool Collider::GetIsTrigger() const
    {
        return _internal->GetIsTrigger();
    }

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

    void Collider::SetMass(float mass)
    {
        _internal->SetMass(mass);
    }

    float Collider::GetMass() const
    {
        return _internal->GetMass();
    }

    void Collider::SetCenter(const Vector3& center)
    {
        _internal->SetCenter(center);
    }

    const Vector3& Collider::GetCenter() const
    {
        return _internal->GetCenter();
    }

    void Collider::SetCollisionReportMode(CollisionReportMode mode)
    {
        _internal->SetCollisionReportMode(mode);
    }

    CollisionReportMode Collider::GetCollisionReportMode() const
    {
        return _internal->GetCollisionReportMode();
    }
}
