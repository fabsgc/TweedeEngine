#include "Components/TeCHeightFieldCollider.h"
#include "Components/TeCBody.h"
#include "Scene/TeSceneObject.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CHeightFieldCollider::CHeightFieldCollider()
        : CCollider((UINT32)TID_CHeightFieldCollider)
    {
        SetName("HeightFieldCollider");
    }

    CHeightFieldCollider::CHeightFieldCollider(const HSceneObject& parent)
        : CCollider(parent, (UINT32)TID_CHeightFieldCollider)
    {
        SetName("HeightFieldCollider");
    }

    SPtr<Collider> CHeightFieldCollider::CreateInternal()
    {
        const SPtr<SceneInstance>& scene = SO()->GetScene();

        SPtr<HeightFieldCollider> collider = HeightFieldCollider::Create(*scene->GetPhysicsScene());
        collider->SetOwner(PhysicsOwnerType::Component, this);

        return collider;
    }

    void CHeightFieldCollider::RestoreInternal()
    {
        CCollider::RestoreInternal();
    }

    void CHeightFieldCollider::Clone(const HHeightFieldCollider& c)
    {
        CCollider::Clone(static_object_cast<CCollider>(c));

        _heightField = c->_heightField;
        _heightScale = c->_heightScale;
    }

    void CHeightFieldCollider::SetHeightField(const HPhysicsHeightField& heightField)
    {
        if (_heightField == heightField)
            return;

        _heightField = heightField;

        if (_internal != nullptr)
        {
            if (_parent != nullptr)
                _parent->RemoveCollider(static_object_cast<CCollider>(GetHandle()));

            _getInternal()->SetHeightField(heightField);

            if (_parent.Empty() || !_heightField.IsLoaded())
                UpdateParentBody();
            else
                _parent->AddCollider(static_object_cast<CCollider>(GetHandle()));
        }
    }

    void CHeightFieldCollider::SetHeightScale(const float& heightScale)
    {
        if (_heightScale == heightScale)
            return;

        _heightScale = heightScale;

        if (_internal != nullptr)
        {
            if (_parent != nullptr)
                _parent->RemoveCollider(static_object_cast<CCollider>(GetHandle()));

            _getInternal()->SetHeightScale(heightScale);

            if (_parent)
                _parent->AddCollider(static_object_cast<CCollider>(GetHandle()));
        }
    }
}
