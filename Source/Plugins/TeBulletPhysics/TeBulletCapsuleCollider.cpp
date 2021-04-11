#include "TeBulletCapsuleCollider.h"
#include "TeBulletPhysics.h"
#include "TeBulletFCollider.h"

namespace te
{
    BulletCapsuleCollider::BulletCapsuleCollider(BulletPhysics* physics, BulletScene* scene, const Vector3& position,
        const Quaternion& rotation, float radius, float height)
        : _radius(radius)
        , _height(height)
    {
        UpdateShape();
        _internal = te_new<BulletFCollider>(physics, scene);
        _internal->SetPosition(position);
        _internal->SetRotation(rotation);
    }

    BulletCapsuleCollider::~BulletCapsuleCollider()
    { 
        te_delete(_internal);
        te_delete(_shape);
    }

    void BulletCapsuleCollider::SetScale(const Vector3& scale)
    {
        CapsuleCollider::SetScale(scale);
        UpdateShape();
    }

    void BulletCapsuleCollider::SetRadius(float radius)
    {
        _radius = radius;
        UpdateShape();
    }

    void BulletCapsuleCollider::SetHeight(float height)
    {
        _height = height;
        UpdateShape();
    }

    void BulletCapsuleCollider::UpdateShape()
    {
        if (!_shape)
        {
            _shape = te_new<btCapsuleShape>(_radius, _height);
            _shape->setUserPointer(this);
        }
        else
        {
            _shape->setImplicitShapeDimensions(btVector3(_radius, 0.5f * _height, _radius));
        }

        _shape->setLocalScaling(ToBtVector3(_internal ? _internal->GetScale() : Vector3::ONE));
    }
}
