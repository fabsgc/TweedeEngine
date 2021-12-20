#include "TeBulletMeshCollider.h"
#include "TeBulletPhysics.h"
#include "TeBulletFCollider.h"
#include "TeBulletMesh.h"
#include "TeBulletFMesh.h"

namespace te
{
    BulletMeshCollider::BulletMeshCollider(BulletPhysics* physics, BulletScene* scene, const Vector3& position, const Quaternion& rotation)
        : BulletCollider(physics, scene)
    {
        _internal = te_new<BulletFCollider>(physics, scene, _shape);
        _internal->SetPosition(position);
        _internal->SetRotation(rotation);

        UpdateCollider();
    }

    BulletMeshCollider::~BulletMeshCollider()
    { 
        te_delete((BulletFCollider*)_internal);
        te_safe_delete(_shape);
    }

    void BulletMeshCollider::SetScale(const Vector3& scale)
    {
        MeshCollider::SetScale(scale);
        UpdateCollider();
    }

    void BulletMeshCollider::SetMesh(const HPhysicsMesh& mesh)
    {
        MeshCollider::SetMesh(mesh);
        UpdateCollider();
    }

    void BulletMeshCollider::SetCollisionType(PhysicsMeshType type)
    {
        MeshCollider::SetCollisionType(type);
        UpdateCollider();
    }

    void BulletMeshCollider::UpdateCollider()
    {
        if (_shape)
        {
            te_delete(_shape);
            _shape = nullptr;
            ((BulletFCollider*)_internal)->SetShape(_shape);
        }

        if (!_mesh.IsLoaded())
            return;

        BulletFMesh* fMesh = static_cast<BulletFMesh*>(_mesh->GetInternal());
        if (!fMesh)
        {
            TE_DEBUG("No data inside the PhysicsMesh");
            return;
        }

        if(_collisionType == PhysicsMeshType::Convex)
        {
            const SPtr<BulletMesh::MeshInfo> mesh = fMesh->GetMeshInfo();

            if (!mesh)
            {
                TE_DEBUG("PhysicsMesh does not have any Mesh Data");
                return;
            }
            
            _shape = te_new<btConvexHullShape>();
            btConvexHullShape* hullShape = (btConvexHullShape*)_shape;
            btScalar* vertices = mesh->Vertices;
            int* indices = mesh->Indices;

            for (UINT32 i = 0; i < mesh->NumIndices; i++)
            {
                const btVector3 position(vertices[indices[i + 0] * 3], vertices[indices[i + 0] * 3 + 1], vertices[indices[i + 0] * 3 + 2]);
                hullShape->addPoint(position, false);
            }

            _shape->setUserPointer(this);
            hullShape->recalcLocalAabb();
            hullShape->optimizeConvexHull();
            hullShape->initializePolyhedralFeatures();

            ((BulletFCollider*)_internal)->SetShape(_shape);
            _shape->setLocalScaling(ToBtVector3(_internal ? _internal->GetScale() : Vector3::ONE));
        }
        else
        {
            const SPtr<BulletMesh::MeshInfo> mesh = fMesh->GetMeshInfo();

            if (!mesh)
            {
                TE_DEBUG("PhysicsMesh does not have any Mesh Data");
                return;
            }

            btTriangleMesh* meshInterface = new btTriangleMesh();
            btScalar* vertices = mesh->Vertices;
            int* indices = mesh->Indices;

            for (UINT32 i = 0; i < mesh->NumIndices; i+=3)
            {
                const btVector3 v0(vertices[indices[i + 0] * 3], vertices[indices[i + 0] * 3 + 1], vertices[indices[i + 0] * 3 + 2]);
                const btVector3 v1(vertices[indices[i + 1] * 3], vertices[indices[i + 1] * 3 + 1], vertices[indices[i + 1] * 3 + 2]);
                const btVector3 v2(vertices[indices[i + 2] * 3], vertices[indices[i + 2] * 3 + 1], vertices[indices[i + 2] * 3 + 2]);
                meshInterface->addTriangle(v0, v1, v2);
            }

            _shape = te_new<btBvhTriangleMeshShape>(meshInterface, true, true);
            _shape->setUserPointer(this);

            ((BulletFCollider*)_internal)->SetShape(_shape);
            _shape->setLocalScaling(ToBtVector3(_internal ? _internal->GetScale() : Vector3::ONE));
        }
    }

    void BulletMeshCollider::OnMeshChanged()
    {
        UpdateCollider();
    }
}
