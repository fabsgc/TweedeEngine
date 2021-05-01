#include "TeBulletMesh.h"
#include "Mesh/TeMeshData.h"
#include "RenderAPI/TeVertexDataDesc.h"
#include "TeBulletPhysics.h"
#include "Math/TeAABox.h"

namespace te
{
    BulletMesh::BulletMesh(const SPtr<MeshData>& meshData)
        : PhysicsMesh(meshData)
    { }

    void BulletMesh::Initialize()
    {
        if (_internal == nullptr) // Could be not-null if we're deserializing
            _internal = te_shared_ptr_new<BulletFMesh>(_initMeshData);

        PhysicsMesh::Initialize();
    }

    void BulletMesh::Destroy()
    {
        _internal = nullptr;
        PhysicsMesh::Destroy();
    }

    BulletFMesh::BulletFMesh()
        : FPhysicsMesh(nullptr, TID_FBulletMesh)
    { }

    BulletFMesh::BulletFMesh(const SPtr<MeshData>& meshData)
        : FPhysicsMesh(meshData, TID_FBulletMesh)
    {
        Initialize();
    }

    BulletFMesh::~BulletFMesh()
    {
        _convexMesh = nullptr;
        _triangleMesh = nullptr;
    }

    void BulletFMesh::Initialize()
    {
        if (!_meshData)
        {
            TE_DEBUG("Provided PhysicsMesh does not have any mesh data.");
            return;
        }

        SPtr<VertexDataDesc> vertexDesc = _meshData->GetVertexDesc();
        if (!vertexDesc->HasElement(VES_POSITION))
        {
            TE_DEBUG("Provided PhysicsMesh mesh data has no vertex positions.");
            return;
        }

        if (_meshData != nullptr)
        {
            // ConvexMesh
            {
                _convexMesh = te_shared_ptr_new<BulletMesh::ConvexMesh>();

                _convexMesh->NumVertices = _meshData->GetNumVertices();
                _convexMesh->Stride = vertexDesc->GetVertexStride();
                _convexMesh->Data = _meshData->GetElementData(VES_POSITION);
            }

            // TriangleMesh
            {
                _triangleMesh = te_shared_ptr_new<BulletMesh::TriangleMesh>();

                _triangleMesh->NumVertices = _meshData->GetNumVertices();
                _triangleMesh->NumIndices = _meshData->GetNumIndices();
                _triangleMesh->VerticeStride = vertexDesc->GetVertexStride();
                _triangleMesh->IndexStride = _meshData->GetIndexElementSize();
                _triangleMesh->Use32BitIndex = (_meshData->GetIndexType() == IndexType::IT_32BIT);
                _triangleMesh->Data = _meshData->GetElementData(VES_POSITION);
                _triangleMesh->Indices = (_triangleMesh->Use32BitIndex) 
                    ? (UINT8*)_meshData->GetIndices32() : (UINT8*)_meshData->GetIndices16();
            }
        }
    }
}
