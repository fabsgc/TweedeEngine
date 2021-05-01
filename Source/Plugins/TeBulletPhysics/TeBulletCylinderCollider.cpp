#include "TeBulletCylinderCollider.h"
#include "TeBulletPhysics.h"
#include "TeBulletFCollider.h"

namespace te
{
    BulletCylinderCollider::BulletCylinderCollider(BulletPhysics* physics, BulletScene* scene, const Vector3& position,
        const Quaternion& rotation, const Vector3& extents)
        : _extents(extents)
    {
        _internal = te_new<BulletFCollider>(physics, scene);
        _internal->SetPosition(position);
        _internal->SetRotation(rotation);

        UpdateShape();
    }

    BulletCylinderCollider::~BulletCylinderCollider()
    { 
        te_delete((BulletFCollider*)_internal);
        te_safe_delete((btCylinderShape*)_shape);
    }

    void BulletCylinderCollider::SetScale(const Vector3& scale)
    {
        CylinderCollider::SetScale(scale);
        UpdateShape();
    }

    void BulletCylinderCollider::SetExtents(const Vector3& extents)
    {
        _extents = extents;
        UpdateShape();
    }

    void BulletCylinderCollider::UpdateShape()
    {
        if (_shape)
            te_delete(_shape);

        _shape = te_new<btCylinderShape>(ToBtVector3(_extents));
        _shape->setUserPointer(this);

        ((BulletFCollider*)_internal)->SetShape(_shape);
        _shape->setLocalScaling(ToBtVector3(_internal ? _internal->GetScale() : Vector3::ONE));
    }
}
