#include "Components/TeCCapsuleCollider.h"
#include "Components/TeCRigidBody.h"
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

        SPtr<CapsuleCollider> collider = CapsuleCollider::Create(*scene->GetPhysicsScene());
        collider->SetOwner(PhysicsOwnerType::Component, this);

        return collider;
    }

    bool CCapsuleCollider::Clone(const HComponent& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        return Clone(static_object_cast<CCapsuleCollider>(c), suffix);
    }

    bool CCapsuleCollider::Clone(const HCapsuleCollider& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        if (CCollider::Clone(static_object_cast<CCollider>(c), suffix))
        {
            _radius = c->_radius;
            _height = c->_height;

            return true;
        }

        return false;
    }

    void CCapsuleCollider::RestoreInternal()
    {
        CCollider::RestoreInternal();

        SetRadius(_radius);
        SetHeight(_height);
    }

    void CCapsuleCollider::SetRadius(float radius)
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

    void CCapsuleCollider::SetHeight(float height)
    {
        float clampedHeight = std::max(height, 0.01f);
        _height = clampedHeight;

        if (_internal != nullptr)
        {
            if (_parent != nullptr)
                _parent->RemoveCollider(static_object_cast<CCollider>(GetHandle()));

            GetInternal()->SetHeight(clampedHeight);

            if (_parent != nullptr)
                _parent->AddCollider(static_object_cast<CCollider>(GetHandle()));
        }
    }
}
