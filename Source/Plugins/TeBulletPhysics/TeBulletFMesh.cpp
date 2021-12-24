#include "TeBulletFMesh.h"

#include "Mesh/TeMeshData.h"
#include "RenderAPI/TeVertexDataDesc.h"
#include "TeBulletPhysics.h"
#include "TeBulletFMesh.h"
#include "Math/TeAABox.h"

namespace te
{
    BulletFMesh::BulletFMesh(const SPtr<MeshData>& meshData)
        : FPhysicsMesh(meshData)
    {
        Initialize();
    }

    BulletFMesh::~BulletFMesh()
    {
        if (_meshInfo)
        {
            te_deallocate(_meshInfo->Vertices);
            te_deallocate(_meshInfo->Indices);
        }
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
            _meshInfo = te_shared_ptr_new<BulletMesh::MeshInfo>();

            UINT32 numVertices = _meshData->GetNumVertices();
            UINT32 numIndices = _meshData->GetNumIndices();
            UINT32 vertexStride = vertexDesc->GetVertexStride();
            UINT32 indexStride = _meshData->GetIndexElementSize();
            UINT8* vertexReader = _meshData->GetElementData(VES_POSITION);
            UINT8* indices = (indexStride == sizeof(UINT32))
                ? (UINT8*)_meshData->GetIndices32() : (UINT8*)_meshData->GetIndices16();

            _meshInfo->NumTriangles = numIndices / 3;
            _meshInfo->NumVertices = numVertices;
            _meshInfo->NumIndices = numIndices;
            _meshInfo->Vertices = te_allocateN<btScalar>(numVertices * 3);
            _meshInfo->Indices = te_allocateN<int>(numIndices);

            for (UINT32 i = 0; i < numVertices; i++)
            {
                Vector3* currVertex = (Vector3*)(vertexReader + (UINT8)vertexStride * i);

                _meshInfo->Vertices[i * 3] = btScalar(currVertex->x);
                _meshInfo->Vertices[i * 3 + 1] = btScalar(currVertex->y);
                _meshInfo->Vertices[i * 3 + 2] = btScalar(currVertex->z);
            }

            for (UINT32 i = 0; i < numIndices; i++)
            {
                if (indexStride == sizeof(UINT32))
                    _meshInfo->Indices[i] = ((UINT32*)(indices))[i];
                else
                    _meshInfo->Indices[i] = ((UINT16*)(indices))[i];
            }            
        }
    }
}
