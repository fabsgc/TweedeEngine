#include "TeBulletBoxCollider.h"
#include "TeBulletPhysics.h"
#include "TeBulletFCollider.h"

namespace te
{
    BulletBoxCollider::BulletBoxCollider(BulletPhysics* physics, BulletScene* scene, const Vector3& position,
        const Quaternion& rotation, const Vector3& extents)
        : _extents(extents)
    {
        UpdateShape();

        _internal = te_new<BulletFCollider>(physics, scene, _shape);
    }

    BulletBoxCollider::~BulletBoxCollider()
    { 
        te_delete(_internal);
        te_delete(_shape);
    }

    void BulletBoxCollider::SetScale(const Vector3& scale)
    {
        BoxCollider::SetScale(scale);
        UpdateShape();
    }

    void BulletBoxCollider::SetExtents(const Vector3& extents)
    {
        _extents = extents;
        UpdateShape();
    }

    Vector3 BulletBoxCollider::GetExtents() const
    {
        return _extents;
    }

    void BulletBoxCollider::UpdateShape()
    {
        if (!_shape)
        {
            _shape = te_new<btBoxShape>(ToBtVector3(_extents));
            _shape->setUserPointer(this);
        }
        else
            _shape->setImplicitShapeDimensions(ToBtVector3(_extents));

        _shape->setLocalScaling(ToBtVector3(_scale));
    }
}
