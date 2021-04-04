#include "Physics/TeCollider.h"
#include "Physics/TePhysics.h"
#include "Physics/TeFCollider.h"

namespace te
{ 
    void Collider::SetScale(const Vector3& scale)
    {
        _scale = scale;
    }

    Vector3 Collider::GetScale() const
    {
        return _scale;
    }

    void Collider::SetIsTrigger(bool value)
    {
        _internal->SetIsTrigger(value);
    }

    bool Collider::GetIsTrigger() const
    {
        return _internal->GetIsTrigger();
    }

    void Collider::SetRigidBody(RigidBody* value)
    {
        _internal->SetIsStatic(value == nullptr);
        _rigidBody = value;
    }
}
