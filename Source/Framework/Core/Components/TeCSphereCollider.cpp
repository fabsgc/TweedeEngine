#include "Components/TeCSphereCollider.h"
#include "Components/TeCBody.h"
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
        const Transform& tfrm = SO()->GetTransform();

        SPtr<Collider> collider = SphereCollider::Create(*scene->GetPhysicsScene(), _radius, tfrm.GetPosition(), tfrm.GetRotation());
        collider->SetOwner(PhysicsOwnerType::Component, this);

        return collider;
    }

    void CSphereCollider::Clone(const HSphereCollider& c)
    {
        CCollider::Clone(static_object_cast<CCollider>(c));
    }

    void CSphereCollider::SetRadius(float radius)
    {
        float clampedRadius = std::max(radius, 0.01f);
        if (_radius == clampedRadius)
            return;

        _radius = clampedRadius;

        if (_internal != nullptr)
        {
            _getInternal()->SetRadius(clampedRadius);

            if (_parent != nullptr)
                _parent->UpdateMassDistribution();
        }
    }
}
