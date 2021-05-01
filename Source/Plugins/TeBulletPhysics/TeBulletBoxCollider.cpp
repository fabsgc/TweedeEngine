#include "TeBulletBoxCollider.h"
#include "TeBulletPhysics.h"
#include "TeBulletFCollider.h"

namespace te
{
    BulletBoxCollider::BulletBoxCollider(BulletPhysics* physics, BulletScene* scene, const Vector3& position,
        const Quaternion& rotation, const Vector3& extents)
        : _extents(extents)
    {
        _internal = te_new<BulletFCollider>(physics, scene);
        _internal->SetPosition(position);
        _internal->SetRotation(rotation);

        UpdateShape();
    }

    BulletBoxCollider::~BulletBoxCollider()
    { 
        te_delete((BulletFCollider*)_internal);
        te_safe_delete((btBoxShape*)_shape);
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

    void BulletBoxCollider::UpdateShape()
    {
        if (_shape)
            te_delete(_shape);

        _shape = te_new<btBoxShape>(ToBtVector3(_extents));
        _shape->setUserPointer(this);

        ((BulletFCollider*)_internal)->SetShape(_shape);
        _shape->setLocalScaling(ToBtVector3(_internal ? _internal->GetScale() : Vector3::ONE));
    }
}
