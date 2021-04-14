#include "Components/TeCCapsuleCollider.h"
#include "Components/TeCBody.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CCapsuleCollider::CCapsuleCollider()
        : CCollider((UINT32)TID_CCapsuleCollider)
    {
        SetName("CapsuleCollider");
    }

    CCapsuleCollider::CCapsuleCollider(const HSceneObject& parent)
        : CCollider(parent, (UINT32)TID_CCapsuleCollider)
    {
        SetName("CapsuleCollider");
    }

    SPtr<Collider> CCapsuleCollider::CreateInternal()
    {
        const SPtr<SceneInstance>& scene = SO()->GetScene();

        SPtr<Collider> collider = CapsuleCollider::Create(*scene->GetPhysicsScene());
        collider->SetOwner(PhysicsOwnerType::Component, this);

        return collider;
    }

    void CCapsuleCollider::Clone(const HCapsuleCollider& c)
    {
        CCollider::Clone(static_object_cast<CCollider>(c));
    }

    void CCapsuleCollider::SetRadius(float radius)
    {
        float clampedRadius = std::max(radius, 0.01f);
        if (_radius == clampedRadius)
            return;

        _radius = clampedRadius;

        if (_internal != nullptr)
        {
            if (_parent != nullptr)
                _parent->RemoveCollider(static_object_cast<CCollider>(GetHandle()));

            _getInternal()->SetRadius(clampedRadius);

            if (_parent != nullptr)
                _parent->AddCollider(static_object_cast<CCollider>(GetHandle()));
        }
    }

    void CCapsuleCollider::SetHeight(float height)
    {
        float clampedHeight = std::max(height, 0.01f);
        if (_height == clampedHeight)
            return;

        _height = clampedHeight;

        if (_internal != nullptr)
        {
            if (_parent != nullptr)
                _parent->RemoveCollider(static_object_cast<CCollider>(GetHandle()));

            _getInternal()->SetHeight(clampedHeight);

            if (_parent != nullptr)
                _parent->AddCollider(static_object_cast<CCollider>(GetHandle()));
        }
    }
}
