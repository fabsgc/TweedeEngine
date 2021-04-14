#include "TeBulletPlaneCollider.h"
#include "TeBulletPhysics.h"
#include "TeBulletFCollider.h"

namespace te
{
    BulletPlaneCollider::BulletPlaneCollider(BulletPhysics* physics, BulletScene* scene, const Vector3& position,
        const Quaternion& rotation, const Vector3 normal)
        : _normal(normal)
    {
        _internal = te_new<BulletFCollider>(physics, scene);
        _internal->SetPosition(position);
        _internal->SetRotation(rotation);

        UpdateShape();
    }

    BulletPlaneCollider::~BulletPlaneCollider()
    { 
        te_delete((BulletFCollider*)_internal);
        te_delete((btStaticPlaneShape*)_shape);
    }

    void BulletPlaneCollider::SetScale(const Vector3& scale)
    {
        PlaneCollider::SetScale(scale);
        UpdateShape();
    }

    void BulletPlaneCollider::SetNormal(const Vector3& normal)
    {
        _normal = normal;
        UpdateShape();
    }

    void BulletPlaneCollider::UpdateShape()
    {
        if (_shape)
            te_delete(_shape);

        _shape = te_new<btStaticPlaneShape>(ToBtVector3(_normal), 0.0f);
        _shape->setUserPointer(this);

        ((BulletFCollider*)_internal)->SetShape(_shape);
        _shape->setLocalScaling(ToBtVector3(_internal ? _internal->GetScale() : Vector3::ONE));
    }
}
