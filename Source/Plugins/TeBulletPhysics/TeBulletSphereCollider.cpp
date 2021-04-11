#include "TeBulletSphereCollider.h"
#include "TeBulletPhysics.h"
#include "TeBulletFCollider.h"

namespace te
{
    BulletSphereCollider::BulletSphereCollider(BulletPhysics* physics, BulletScene* scene, const Vector3& position,
        const Quaternion& rotation, float radius)
        : _radius(radius)
    {
        UpdateShape();
        _internal = te_new<BulletFCollider>(physics, scene, _shape);
    }

    BulletSphereCollider::~BulletSphereCollider()
    { 
        te_delete(_internal);
        te_delete(_shape);
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
        if (!_shape)
        {
            _shape = te_new<btSphereShape>(_radius);
            _shape->setUserPointer(this);
        }
        else
        {
            _shape->setImplicitShapeDimensions(btVector3(_radius, 0, 0));
        }

        _shape->setLocalScaling(ToBtVector3(_internal ? _internal->GetScale() : Vector3::ONE));
    }
}
