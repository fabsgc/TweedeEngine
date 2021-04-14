#include "TeBulletSphereCollider.h"
#include "TeBulletPhysics.h"
#include "TeBulletFCollider.h"

namespace te
{
    BulletSphereCollider::BulletSphereCollider(BulletPhysics* physics, BulletScene* scene, const Vector3& position,
        const Quaternion& rotation, float radius)
        : _radius(radius)
    {
        _internal = te_new<BulletFCollider>(physics, scene);
        _internal->SetPosition(position);
        _internal->SetRotation(rotation);

        UpdateShape();
    }

    BulletSphereCollider::~BulletSphereCollider()
    { 
        te_delete((BulletFCollider*)_internal);
        te_delete((btSphereShape*)_shape);
    }

    void BulletSphereCollider::SetScale(const Vector3& scale)
    {
        SphereCollider::SetScale(scale);
        UpdateShape();
    }

    void BulletSphereCollider::SetRadius(float radius)
    {
        _radius = radius;
        UpdateShape();
    }

    void BulletSphereCollider::UpdateShape()
    {
        if (_shape)
            te_delete(_shape);

        _shape = te_new<btSphereShape>(_radius);
        _shape->setUserPointer(this);

        ((BulletFCollider*)_internal)->SetShape(_shape);
        _shape->setLocalScaling(ToBtVector3(_internal ? _internal->GetScale() : Vector3::ONE));
    }
}
