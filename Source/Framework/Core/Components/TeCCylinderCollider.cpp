#include "Components/TeCCylinderCollider.h"
#include "Components/TeCBody.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CCylinderCollider::CCylinderCollider()
        : CCollider((UINT32)TID_CCylinderCollider)
    {
        SetName("CylinderCollider");
    }

    CCylinderCollider::CCylinderCollider(const HSceneObject& parent)
        : CCollider(parent, (UINT32)TID_CCylinderCollider)
    {
        SetName("CylinderCollider");
    }

    SPtr<Collider> CCylinderCollider::CreateInternal()
    {
        const SPtr<SceneInstance>& scene = SO()->GetScene();

        SPtr<Collider> collider = CylinderCollider::Create(*scene->GetPhysicsScene());
        collider->SetOwner(PhysicsOwnerType::Component, this);

        return collider;
    }

    void CCylinderCollider::Clone(const HCylinderCollider& c)
    {
        CCollider::Clone(static_object_cast<CCollider>(c));
    }

    void CCylinderCollider::SetExtents(const Vector3& extents)
    {
        Vector3 clampedExtents = Vector3::Max(extents, Vector3(0.01f, 0.01f, 0.01f));

        if (_extents == clampedExtents)
            return;

        _extents = clampedExtents;

        if (_internal != nullptr)
        {
            if (_parent != nullptr)
                _parent->RemoveCollider(static_object_cast<CCollider>(GetHandle()));

            _getInternal()->SetExtents(clampedExtents);

            if (_parent != nullptr)
                _parent->AddCollider(static_object_cast<CCollider>(GetHandle()));
        }
    }
}
