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
        if (_shape)
        {
            te_delete(_shape);
            _shape = nullptr;
        }

        if (!_mesh.IsLoaded())
            return;

        BulletFMesh* fMesh = static_cast<BulletFMesh*>(_mesh->_getInternal());
        if (!fMesh)
        {
            TE_DEBUG("No data inside the PhysicsMesh");
            return;
        }

        if(_collisionType == PhysicsMeshType::Convex)
        {
            const SPtr<BulletMesh::ConvexMesh> convexMesh = fMesh->GetConvexMesh();

            if (!convexMesh)
            {
                TE_DEBUG("PhysicsMesh does not have any ConvexMesh Data");
                return;
            }
            
            _shape = te_new<btConvexHullShape>();
            btConvexHullShape* hullShape = (btConvexHullShape*)_shape;

            for (UINT32 i = 0; i < convexMesh->NumVertices; i++)
            {
                Vector3 position = *(Vector3*)(convexMesh->Vertices + convexMesh->Stride * i);
                hullShape->addPoint(ToBtVector3(position));
            }

            hullShape->setUserPointer(this);
            hullShape->optimizeConvexHull();
            hullShape->initializePolyhedralFeatures();

            ((BulletFCollider*)_internal)->SetShape(_shape);
            _shape->setLocalScaling(ToBtVector3(_internal ? _internal->GetScale() : Vector3::ONE));
        }
        else
        {
            const SPtr<BulletMesh::TriangleMesh> triangleMesh = fMesh->GetTriangleMesh();

            if (!triangleMesh)
            {
                TE_DEBUG("PhysicsMesh does not have any TriangleMesh Data");
                return;
            }

            btTriangleMesh* meshInterface = new btTriangleMesh();
            /*btIndexedMesh part;

            part.m_vertexBase = (const unsigned char*)triangleMesh->Vertices;
            part.m_vertexStride = triangleMesh->VertexStride;
            part.m_numVertices = triangleMesh->NumVertices;
            part.m_triangleIndexBase = (const unsigned char*)triangleMesh->Indices;
            part.m_triangleIndexStride = triangleMesh->IndexStride;
            part.m_numTriangles = triangleMesh->NumIndices / 3;
            part.m_vertexType = PHY_FLOAT;
            part.m_indexType = triangleMesh->Use32BitIndex ? PHY_INTEGER : PHY_SHORT;
            
            meshInterface->addIndexedMesh(part, part.m_indexType);*/

            for (UINT32 i = 0; i < triangleMesh->NumIndices / 3; i++)
            {
                const btVector3& v0 = ToBtVector3(((Vector3*)triangleMesh->Vertices)[((UINT32*)(triangleMesh->Indices))[i * 3]]);
                const btVector3& v1 = ToBtVector3(((Vector3*)triangleMesh->Vertices)[((UINT32*)(triangleMesh->Indices))[i * 3 + 1]]);
                const btVector3& v2 = ToBtVector3(((Vector3*)triangleMesh->Vertices)[((UINT32*)(triangleMesh->Indices))[i * 3 + 2]]);
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
        UpdateShape();
    }
}
