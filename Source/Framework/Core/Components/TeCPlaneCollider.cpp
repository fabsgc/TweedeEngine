#include "Components/TeCPlaneCollider.h"
#include "Components/TeCRigidBody.h"
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

    bool CPlaneCollider::IsValidParent(const HRigidBody& parent) const
    {
        // Planes cannot be added to non-kinematic rigidbodies
        return parent->GetIsKinematic();
    }

    bool CPlaneCollider::Clone(const HPlaneCollider& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        if (CCollider::Clone(static_object_cast<CCollider>(c), suffix))
        {
            _normal = c->_normal;
            return true;
        }

        return false;
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

            GetInternal()->SetNormal(clampedNormal);

            if (_parent != nullptr)
                _parent->AddCollider(static_object_cast<CCollider>(GetHandle()));
        }
    }
}
