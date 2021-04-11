#include "TeBulletCylinderCollider.h"
#include "TeBulletPhysics.h"
#include "TeBulletFCollider.h"

namespace te
{
    BulletCylinderCollider::BulletCylinderCollider(BulletPhysics* physics, BulletScene* scene, const Vector3& position,
        const Quaternion& rotation, const Vector3& extents)
        : _extents(extents)
    {
        UpdateShape();
        _internal = te_new<BulletFCollider>(physics, scene, _shape);
        _internal->SetPosition(position);
        _internal->SetRotation(rotation);
    }

    BulletCylinderCollider::~BulletCylinderCollider()
    { 
        te_delete(_internal);
        te_delete(_shape);
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
        if (!_shape)
        {
            _shape = te_new<btCylinderShape>(ToBtVector3(_extents));
            _shape->setUserPointer(this);
        }
        else
        {
            _shape->setImplicitShapeDimensions(ToBtVector3(_extents));
        }

        _shape->setLocalScaling(ToBtVector3(_internal ? _internal->GetScale() : Vector3::ONE));
    }
}
