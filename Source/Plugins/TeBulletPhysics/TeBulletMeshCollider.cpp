#include "TeBulletMeshCollider.h"
#include "TeBulletPhysics.h"
#include "TeBulletFCollider.h"

namespace te
{
    BulletMeshCollider::BulletMeshCollider(BulletPhysics* physics, BulletScene* scene, const Vector3& position, const Quaternion& rotation)
    {
        _internal = te_new<BulletFCollider>(physics, scene, _shape);
        _internal->SetPosition(position);
        _internal->SetRotation(rotation);

        UpdateShape();
    }

    BulletMeshCollider::~BulletMeshCollider()
    { 
        te_delete((BulletFCollider*)_internal);
        te_delete(_shape);
    }

    void BulletMeshCollider::SetScale(const Vector3& scale)
    {
        MeshCollider::SetScale(scale);
        UpdateShape();
    }

    void BulletMeshCollider::UpdateShape()
    {
        if (_shape)
            te_delete(_shape);

        /*_shape = te_new<btConvexHullShape>(); // TODO
        _shape->setUserPointer(this);

        ((BulletFCollider*)_internal)->SetShape(_shape);
        _shape->setLocalScaling(ToBtVector3(_internal ? _internal->GetScale() : Vector3::ONE));*/
    }

    void BulletMeshCollider::OnMeshChanged()
    {
        ApplyGeometry();
    }

    void BulletMeshCollider::ApplyGeometry()
    {
        // TODO

        UpdateShape();
    }
}
