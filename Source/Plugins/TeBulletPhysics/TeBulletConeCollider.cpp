#include "TeBulletConeCollider.h"
#include "TeBulletPhysics.h"
#include "TeBulletFCollider.h"

namespace te
{
    BulletConeCollider::BulletConeCollider(BulletPhysics* physics, BulletScene* scene, const Vector3& position,
        const Quaternion& rotation, float radius, float height)
        : BulletCollider(physics, scene)
        ,_radius(radius)
        , _height(height)
    {
        _internal = te_new<BulletFCollider>(physics, scene);
        _internal->SetPosition(position);
        _internal->SetRotation(rotation);

        UpdateCollider();
    }

    BulletConeCollider::~BulletConeCollider()
    { 
        te_delete((BulletFCollider*)_internal);
        te_safe_delete((btConeShape*)_shape);
    }

    void BulletConeCollider::SetScale(const Vector3& scale)
    {
        ConeCollider::SetScale(scale);
        UpdateCollider();
    }

    void BulletConeCollider::SetRadius(float radius)
    {
        _radius = radius;
        UpdateCollider();
    }

    void BulletConeCollider::SetHeight(float height)
    {
        _height = height;
        UpdateCollider();
    }

    void BulletConeCollider::UpdateCollider()
    {
        if (_shape)
            te_delete(_shape);

        _shape = te_new<btConeShape>(_radius, _height);
        _shape->setUserPointer(this);
        
        ((BulletFCollider*)_internal)->SetShape(_shape);
        _shape->setLocalScaling(ToBtVector3(_internal ? _internal->GetScale() : Vector3::ONE));
    }
}
