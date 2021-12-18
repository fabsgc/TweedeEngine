#include "Components/TeCHeightFieldCollider.h"
#include "Components/TeCRigidBody.h"
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
        _minHeight = c->_minHeight;
        _maxHeight = c->_maxHeight;
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

    void CHeightFieldCollider::SetMinHeight(const float& minHeight)
    {
        if (_minHeight == minHeight || minHeight > _maxHeight)
            return;

        _minHeight = minHeight;

        if (_internal != nullptr)
        {
            if (_parent != nullptr)
                _parent->RemoveCollider(static_object_cast<CCollider>(GetHandle()));

            _getInternal()->SetMinHeight(minHeight);

            if (_parent)
                _parent->AddCollider(static_object_cast<CCollider>(GetHandle()));
        }
    }

    void CHeightFieldCollider::SetMaxHeight(const float& maxHeight)
    {
        if (_maxHeight == maxHeight || maxHeight < _minHeight)
            return;

        _maxHeight = maxHeight;

        if (_internal != nullptr)
        {
            if (_parent != nullptr)
                _parent->RemoveCollider(static_object_cast<CCollider>(GetHandle()));

            _getInternal()->SetMaxHeight(maxHeight);

            if (_parent)
                _parent->AddCollider(static_object_cast<CCollider>(GetHandle()));
        }
    }
}
