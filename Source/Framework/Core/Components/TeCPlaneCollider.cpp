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

        SPtr<PlaneCollider> collider = PlaneCollider::Create(*scene->GetPhysicsScene(), _normal);
        collider->SetOwner(PhysicsOwnerType::Component, this);

        return collider;
    }

    void CPlaneCollider::RestoreInternal()
    {
        CCollider::RestoreInternal();

        SetNormal(_normal);
    }

    void CPlaneCollider::Clone(const HPlaneCollider& c)
    {
        CCollider::Clone(static_object_cast<CCollider>(c));

        _normal = c->_normal;
    }

    void CPlaneCollider::SetNormal(const Vector3& normal)
    {
        Vector3 clampedNormal = normal;
        clampedNormal.Normalize();
        _normal = clampedNormal;

        if (_internal != nullptr)
        {
            if (_parent != nullptr)
                _parent->RemoveCollider(static_object_cast<CCollider>(GetHandle()));

            _getInternal()->SetNormal(clampedNormal);

            if (_parent != nullptr)
                _parent->AddCollider(static_object_cast<CCollider>(GetHandle()));
        }
    }
}
