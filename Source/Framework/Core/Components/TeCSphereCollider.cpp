#include "Components/TeCSphereCollider.h"
#include "Components/TeCRigidBody.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CSphereCollider::CSphereCollider()
        : CCollider((UINT32)TID_CSphereCollider)
    {
        SetName("SphereCollider");
    }

    CSphereCollider::CSphereCollider(const HSceneObject& parent, float radius)
        : CCollider(parent, (UINT32)TID_CSphereCollider)
        , _radius(radius)
    {
        SetName("SphereCollider");
    }

    SPtr<Collider> CSphereCollider::CreateInternal()
    {
        const SPtr<SceneInstance>& scene = SO()->GetScene();

        SPtr<SphereCollider> collider = SphereCollider::Create(*scene->GetPhysicsScene(), _radius);
        collider->SetOwner(PhysicsOwnerType::Component, this);

        return collider;
    }

    void CSphereCollider::RestoreInternal()
    {
        CCollider::RestoreInternal();

        SetRadius(_radius);
    }

    void CSphereCollider::Clone(const HSphereCollider& c)
    {
        CCollider::Clone(static_object_cast<CCollider>(c));

        _radius = c->_radius;
    }

    void CSphereCollider::SetRadius(float radius)
    {
        float clampedRadius = std::max(radius, 0.01f);
        _radius = clampedRadius;

        if (_internal != nullptr)
        {
            if (_parent != nullptr)
                _parent->RemoveCollider(static_object_cast<CCollider>(GetHandle()));

            GetInternal()->SetRadius(clampedRadius);

            if (_parent != nullptr)
                _parent->AddCollider(static_object_cast<CCollider>(GetHandle()));
        }
    }
}
