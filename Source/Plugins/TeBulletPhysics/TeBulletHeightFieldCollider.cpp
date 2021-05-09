#include "TeBulletHeightFieldCollider.h"
#include "TeBulletPhysics.h"
#include "TeBulletFCollider.h"
#include "TeBulletHeightField.h"

namespace te
{
    BulletHeightFieldCollider::BulletHeightFieldCollider(BulletPhysics* physics, BulletScene* scene, const Vector3& position, const Quaternion& rotation)
    {
        _internal = te_new<BulletFCollider>(physics, scene, _shape);
        _internal->SetPosition(position);
        _internal->SetRotation(rotation);

        UpdateShape();
    }

    BulletHeightFieldCollider::~BulletHeightFieldCollider()
    { 
        te_delete((BulletFCollider*)_internal);
        te_safe_delete(_shape);
    }

    void BulletHeightFieldCollider::SetScale(const Vector3& scale)
    {
        HeightFieldCollider::SetScale(scale);
        UpdateShape();
    }

    void BulletHeightFieldCollider::UpdateShape()
    {
        if (_shape)
        {
            te_delete(_shape);
            _shape = nullptr;
            ((BulletFCollider*)_internal)->SetShape(_shape);
        }

        if (!_heightField.IsLoaded())
            return;

        BulletFHeightField* fHeightField = static_cast<BulletFHeightField*>(_heightField->_getInternal());
        if (!fHeightField)
        {
            TE_DEBUG("No data inside the PhysicsHeightField");
            return;
        }

        auto heightFieldInfo = fHeightField->GetHeightFieldInfo();

        _shape = te_new<btHeightfieldTerrainShape>(heightFieldInfo->Width, heightFieldInfo->Length, 
            heightFieldInfo->HeightMap, _heightScale, heightFieldInfo->MinHeight, heightFieldInfo->MaxHeight, 1, true);
        _shape->setUserPointer(this);

        ((BulletFCollider*)_internal)->SetShape(_shape);
        _shape->setLocalScaling(ToBtVector3(_internal ? _internal->GetScale() : Vector3::ONE));
    }

    void BulletHeightFieldCollider::OnHeightFieldChanged()
    {
        UpdateShape();
    }
}
