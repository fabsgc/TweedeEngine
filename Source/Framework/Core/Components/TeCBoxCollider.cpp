#include "Components/TeCBoxCollider.h"
#include "Components/TeCBody.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CBoxCollider::CBoxCollider()
        : CCollider((UINT32)TID_CBoxCollider)
    {
        SetName("BoxCollider");
    }

    CBoxCollider::CBoxCollider(const HSceneObject& parent, const Vector3& extents)
        : CCollider(parent, (UINT32)TID_CBoxCollider)
        , _extents(extents)
    {
        SetName("BoxCollider");
    }

    SPtr<Collider> CBoxCollider::CreateInternal()
    {
        const SPtr<SceneInstance>& scene = SO()->GetScene();
        const Transform& tfrm = SO()->GetTransform();

        SPtr<Collider> collider = BoxCollider::Create(*scene->GetPhysicsScene(), _extents, tfrm.GetPosition(), tfrm.GetRotation());
        collider->SetOwner(PhysicsOwnerType::Component, this);

        return collider;
    }

    void CBoxCollider::SetExtents(const Vector3& extents)
    {
        Vector3 clampedExtents = Vector3::Max(extents, Vector3(0.01f, 0.01f, 0.01f));

        if (_extents == clampedExtents)
            return;

        _extents = clampedExtents;

        if (_internal != nullptr)
        {
           _getInternal()->SetExtents(clampedExtents);

            if (_parent != nullptr)
                _parent->UpdateMassDistribution();
        }
    }

    void CBoxCollider::Clone(const HBoxCollider& c)
    {
        CCollider::Clone(static_object_cast<CCollider>(c));
    }
}
