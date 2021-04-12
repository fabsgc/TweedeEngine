#include "Components/TeCConeCollider.h"
#include "Components/TeCBody.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CConeCollider::CConeCollider()
        : CCollider((UINT32)TID_CConeCollider)
    {
        SetName("ConeCollider");
    }

    CConeCollider::CConeCollider(const HSceneObject& parent)
        : CCollider(parent, (UINT32)TID_CConeCollider)
    {
        SetName("ConeCollider");
    }

    SPtr<Collider> CConeCollider::CreateInternal()
    {
        const SPtr<SceneInstance>& scene = SO()->GetScene();
        SPtr<Collider> collider = ConeCollider::Create(*scene->GetPhysicsScene());
        collider->SetOwner(PhysicsOwnerType::Component, this);

        return collider;
    }

    void CConeCollider::Clone(const HConeCollider& c)
    {
        CCollider::Clone(static_object_cast<CCollider>(c));
    }

    void CConeCollider::SetRadius(float radius)
    {
        float clampedRadius = std::max(radius, 0.01f);
        if (_radius == clampedRadius)
            return;

        _radius = clampedRadius;

        if (_internal != nullptr)
        {
            _getInternal()->SetRadius(clampedRadius);

            if (_parent != nullptr)
                _parent->AddCollider(static_object_cast<CCollider>(GetHandle()));
        }
    }

    void CConeCollider::SetHeight(float height)
    {
        float clampedHeight = std::max(height, 0.01f);
        if (_height == clampedHeight)
            return;

        _height = clampedHeight;

        if (_internal != nullptr)
        {
            _getInternal()->SetHeight(clampedHeight);

            if (_parent != nullptr)
                _parent->AddCollider(static_object_cast<CCollider>(GetHandle()));
        }
    }
}
