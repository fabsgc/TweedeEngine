#include "TeBulletHeightFieldCollider.h"
#include "TeBulletPhysics.h"
#include "TeBulletFCollider.h"

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
        te_delete(_shape);
    }

    void BulletHeightFieldCollider::SetScale(const Vector3& scale)
    {
        HeightFieldCollider::SetScale(scale);
        UpdateShape();
    }

    void BulletHeightFieldCollider::UpdateShape()
    {
        if (_shape)
            te_delete(_shape);

        /*_shape = te_new<btHeightfieldTerrainShape>(0, 0, data, 0.0f, 0.0f, 1, true); // TODO
        _shape->setUserPointer(this);

        ((BulletFCollider*)_internal)->SetShape(_shape);
        _shape->setLocalScaling(ToBtVector3(_internal ? _internal->GetScale() : Vector3::ONE));*/
    }
}
