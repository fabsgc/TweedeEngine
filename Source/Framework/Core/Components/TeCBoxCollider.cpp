#include "Components/TeCBoxCollider.h"
#include "Components/TeCRigidBody.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CBoxCollider::CBoxCollider()
        : CCollider((UINT32)TID_CBoxCollider)
    {
        SetName("BoxCollider");
        SetFlag(Component::AlwaysRun, true);
    }

    CBoxCollider::CBoxCollider(const HSceneObject& parent, const Vector3& extents)
        : CCollider(parent, (UINT32)TID_CBoxCollider)
        , _extents(extents)
    {
        SetName("BoxCollider");
        SetFlag(Component::AlwaysRun, true);
    }

    SPtr<Collider> CBoxCollider::CreateInternal()
    {
        const SPtr<SceneInstance>& scene = SO()->GetScene();

        SPtr<BoxCollider> collider = BoxCollider::Create(*scene->GetPhysicsScene(), _extents);
        collider->SetOwner(PhysicsOwnerType::Component, this);

        return collider;
    }

    void CBoxCollider::RestoreInternal()
    {
        CCollider::RestoreInternal();
        SetExtents(_extents);
    }

    bool CBoxCollider::Clone(const HBoxCollider& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        if (CCollider::Clone(static_object_cast<CCollider>(c), suffix))
        {
            _extents = c->_extents;
            return true;
        }

        return false;
    }

    void CBoxCollider::SetExtents(const Vector3& extents)
    {
        Vector3 clampedExtents = Vector3::Max(extents, Vector3(0.01f, 0.01f, 0.01f));
        _extents = clampedExtents;

        if (_internal != nullptr)
        {
            if (_parent != nullptr)
                _parent->RemoveCollider(static_object_cast<CCollider>(GetHandle()));

            GetInternal()->SetExtents(clampedExtents);

            if (_parent != nullptr)
                _parent->AddCollider(static_object_cast<CCollider>(GetHandle()));
        }
    }
}
