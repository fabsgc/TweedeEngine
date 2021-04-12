#include "Components/TeCPlaneCollider.h"
#include "Components/TeCBody.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CPlaneCollider::CPlaneCollider()
        : CCollider((UINT32)TID_CPlaneCollider)
    {
        SetName("PlaneCollider");
    }

    CPlaneCollider::CPlaneCollider(const HSceneObject& parent)
        : CCollider(parent, (UINT32)TID_CPlaneCollider)
    {
        SetName("PlaneCollider");
    }

    SPtr<Collider> CPlaneCollider::CreateInternal()
    {
        const SPtr<SceneInstance>& scene = SO()->GetScene();
        const Transform& tfrm = SO()->GetTransform();

        SPtr<Collider> collider = PlaneCollider::Create(*scene->GetPhysicsScene(), _normal, tfrm.GetPosition(), tfrm.GetRotation());
        collider->SetOwner(PhysicsOwnerType::Component, this);

        return collider;
    }

    void CPlaneCollider::Clone(const HPlaneCollider& c)
    {
        CCollider::Clone(static_object_cast<CCollider>(c));
    }

    void CPlaneCollider::SetNormal(const Vector3& normal)
    {
        Vector3 clampedNormal = normal;
        clampedNormal.Normalize();

        if (_normal == clampedNormal)
            return;

        _normal = clampedNormal;

        if (_internal != nullptr)
        {
            _getInternal()->SetNormal(clampedNormal);

            if (_parent != nullptr)
                _parent->UpdateMassDistribution();
        }
    }
}
