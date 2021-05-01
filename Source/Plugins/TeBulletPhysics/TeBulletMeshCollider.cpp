#include "TeBulletMeshCollider.h"
#include "TeBulletPhysics.h"
#include "TeBulletFCollider.h"
#include "TeBulletMesh.h"

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
        te_safe_delete(_shape);
    }

    void BulletMeshCollider::SetScale(const Vector3& scale)
    {
        MeshCollider::SetScale(scale);
        UpdateShape();
    }

    void BulletMeshCollider::SetCollisionType(PhysicsMeshType type)
    {
        MeshCollider::SetCollisionType(type);
        UpdateShape();
    }

    void BulletMeshCollider::UpdateShape()
    {
        if (!_physicMesh.IsLoaded())
            return;

        if (_shape)
            te_delete(_shape);

        BulletFMesh* fMesh = static_cast<BulletFMesh*>(_physicMesh->_getInternal());
        if (!fMesh)
        {
            TE_DEBUG("No data inside the PhysicMesh");
            return;
        }

        if(_collisionType == PhysicsMeshType::Convex)
        {
            const SPtr<BulletMesh::ConvexMesh> convexMesh = fMesh->GetConvexMesh();

            if (!convexMesh)
            {
                TE_DEBUG("PhysicMesh does not have any ConvexMesh Data");
                return;
            }
            
            _shape = te_new<btConvexHullShape>((btScalar*)convexMesh->Data, convexMesh->NumVertices, convexMesh->Stride);
            _shape->setUserPointer(this);
            _shape->optimizeConvexHull();
            _shape->initializePolyhedralFeatures();

            ((BulletFCollider*)_internal)->SetShape(_shape);
            _shape->setLocalScaling(ToBtVector3(_internal ? _internal->GetScale() : Vector3::ONE));
        }

        // TriangleMesh
        {
            const SPtr<BulletMesh::TriangleMesh> triangleMesh = fMesh->GetTriangleMesh();

            if (!triangleMesh)
            {
                TE_DEBUG("PhysicMesh does not have any TriangleMesh Data");
                return;
            }
        }
    }

    void BulletMeshCollider::OnMeshChanged()
    {
        ApplyGeometry();
    }

    void BulletMeshCollider::ApplyGeometry()
    {
        UpdateShape();
    }
}
