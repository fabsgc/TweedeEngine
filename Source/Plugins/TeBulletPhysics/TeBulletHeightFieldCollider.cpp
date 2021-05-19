#include "TeBulletHeightFieldCollider.h"
#include "TeBulletPhysics.h"
#include "TeBulletFCollider.h"
#include "TeBulletHeightField.h"

namespace te
{
    BulletHeightFieldCollider::BulletHeightFieldCollider(BulletPhysics* physics, BulletScene* scene, const Vector3& position, const Quaternion& rotation)
        : BulletCollider(physics, scene)
    {
        _internal = te_new<BulletFCollider>(physics, scene, _shape);
        _internal->SetPosition(position);
        _internal->SetRotation(rotation);

        UpdateCollider();
    }

    BulletHeightFieldCollider::~BulletHeightFieldCollider()
    { 
        te_delete((BulletFCollider*)_internal);
        te_safe_delete(_shape);
        te_safe_delete(_scaledHeightMap);
    }

    void BulletHeightFieldCollider::SetScale(const Vector3& scale)
    {
        HeightFieldCollider::SetScale(scale);
        UpdateCollider();
    }

    void BulletHeightFieldCollider::SetHeightField(const HPhysicsHeightField& heightField)
    {
        HeightFieldCollider::SetHeightField(heightField);
        UpdateCollider();
    }

    void BulletHeightFieldCollider::SetMinHeight(const float& minHeight)
    {
        HeightFieldCollider::SetMinHeight(minHeight);
        UpdateCollider();
    }

    void BulletHeightFieldCollider::SetMaxHeight(const float& maxHeight)
    {
        HeightFieldCollider::SetMaxHeight(maxHeight);
        UpdateCollider();
    }

    void BulletHeightFieldCollider::UpdateCollider()
    {
        if (_shape)
        {
            te_delete(_shape);
            _shape = nullptr;
            ((BulletFCollider*)_internal)->SetShape(_shape);
        }

        if (_scaledHeightMap)
        {
            te_delete(_scaledHeightMap);
            _scaledHeightMap = nullptr;
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

        UINT32 numElts = heightFieldInfo->Width * heightFieldInfo->Length;
        UINT32 mapSize = sizeof(float) * numElts;
        _scaledHeightMap = te_allocate<UINT8>(mapSize);

        memcpy(_scaledHeightMap, heightFieldInfo->HeightMap, mapSize);

        float oldRange = (1.0f - 0.0f);
        float newRange = (_maxHeight - _minHeight);
        
        for (UINT32 i = 0; i < numElts; i++)
        {
            ((float*)_scaledHeightMap)[i] = ((((float*)heightFieldInfo->HeightMap)[i] * newRange) / oldRange) + _minHeight;
        }

        _shape = te_new<btHeightfieldTerrainShape>(heightFieldInfo->Width, heightFieldInfo->Length, 
            (float*)_scaledHeightMap, _minHeight, _maxHeight, 1, true);
        _shape->setUserPointer(this);

        ((BulletFCollider*)_internal)->SetShape(_shape);
        _shape->setLocalScaling(ToBtVector3(_internal ? _internal->GetScale() : Vector3::ONE));
    }

    void BulletHeightFieldCollider::OnHeightFieldChanged()
    {
        UpdateCollider();
    }
}
