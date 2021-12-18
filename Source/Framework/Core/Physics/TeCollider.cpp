#include "Physics/TeCollider.h"
#include "Physics/TePhysics.h"
#include "Physics/TeBody.h"
#include "Physics/TeFCollider.h"
#include "Math/TeRay.h"

namespace te
{
    Collider::Collider(UINT32 type)
        : Serializable(type)
    { }

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
}
