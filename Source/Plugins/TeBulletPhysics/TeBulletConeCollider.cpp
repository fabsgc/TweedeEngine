#include "TeBulletConeCollider.h"
#include "TeBulletPhysics.h"
#include "TeBulletFCollider.h"

namespace te
{
    BulletConeCollider::BulletConeCollider(BulletPhysics* physics, BulletScene* scene, const Vector3& position,
        const Quaternion& rotation, float radius, float height)
        : _radius(radius)
        , _height(height)
    {
        _internal = te_new<BulletFCollider>(physics, scene);
        _internal->SetPosition(position);
        _internal->SetRotation(rotation);

        UpdateShape();
    }

    BulletConeCollider::~BulletConeCollider()
    { 
        te_delete((BulletFCollider*)_internal);
        te_delete((btConeShape*)_shape);
    }

    void BulletConeCollider::SetScale(const Vector3& scale)
    {
        ConeCollider::SetScale(scale);
        UpdateShape();
    }

    void BulletConeCollider::SetRadius(float radius)
    {
        _radius = radius;
        UpdateShape();
    }

    void BulletConeCollider::SetHeight(float height)
    {
        _height = height;
        UpdateShape();
    }

    void BulletConeCollider::UpdateShape()
    {
        if (_shape)
            te_delete(_shape);

        _shape = te_new<btConeShape>(_radius, _height);
        _shape->setUserPointer(this);
        
        ((BulletFCollider*)_internal)->SetShape(_shape);
        _shape->setLocalScaling(ToBtVector3(_internal ? _internal->GetScale() : Vector3::ONE));
    }
}
