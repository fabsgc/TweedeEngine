#include "TeBulletMeshCollider.h"
#include "TeBulletPhysics.h"
#include "TeBulletFCollider.h"

namespace te
{
    BulletMeshCollider::BulletMeshCollider(BulletPhysics* physics, BulletScene* scene, const Vector3& position, const Quaternion& rotation)
    {
        UpdateShape();
        _internal = te_new<BulletFCollider>(physics, scene);
        _internal->SetPosition(position);
        _internal->SetRotation(rotation);
    }

    BulletMeshCollider::~BulletMeshCollider()
    { 
        te_delete(_internal);
        te_delete(_shape);
    }

    void BulletMeshCollider::SetScale(const Vector3& scale)
    {
        MeshCollider::SetScale(scale);
        UpdateShape();
    }

    void BulletMeshCollider::UpdateShape()
    {
        if (!_shape)
        {
            _shape = te_new<btConvexHullShape>(); // TODO
            _shape->setUserPointer(this);
        }

        _shape->setLocalScaling(ToBtVector3(_internal ? _internal->GetScale() : Vector3::ONE));
    }
}
