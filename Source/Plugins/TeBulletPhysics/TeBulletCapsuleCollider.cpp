#include "TeBulletCapsuleCollider.h"
#include "TeBulletPhysics.h"
#include "TeBulletFCollider.h"

namespace te
{
    BulletCapsuleCollider::BulletCapsuleCollider(BulletPhysics* physics, BulletScene* scene, const Vector3& position,
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

    BulletCapsuleCollider::~BulletCapsuleCollider()
    { 
        te_delete((BulletFCollider*)_internal);
        te_safe_delete((btCapsuleShape*)_shape);
    }

    void BulletCapsuleCollider::SetScale(const Vector3& scale)
    {
        CapsuleCollider::SetScale(scale);
        UpdateCollider();
    }

    void BulletCapsuleCollider::SetRadius(float radius)
    {
        _radius = radius;
        UpdateCollider();
    }

    void BulletCapsuleCollider::SetHeight(float height)
    {
        _height = height;
        UpdateCollider();
    }

    void BulletCapsuleCollider::UpdateCollider()
    {
        if (_shape)
            te_delete(_shape);

        _shape = te_new<btCapsuleShape>(_radius, _height);
        _shape->setUserPointer(this);

        ((BulletFCollider*)_internal)->SetShape(_shape);
        _shape->setLocalScaling(ToBtVector3(_internal ? _internal->GetScale() : Vector3::ONE));
    }
}
