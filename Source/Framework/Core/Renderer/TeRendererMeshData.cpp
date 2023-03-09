#include "Renderer/TeRendererMeshData.h"
#include "RenderAPI/TeVertexDataDesc.h"
#include "Mesh/TeMeshData.h"
#include "Math/TeVector2.h"
#include "Math/TeVector3.h"
#include "Math/TeVector4.h"
#include "Image/TeColor.h"
#include "Image/TePixelUtil.h"
#include "Manager/TeRendererManager.h"
#include "Renderer/TeRenderer.h"
#include "Mesh/TeMeshUtility.h"

namespace te
{
    RendererMeshData::RendererMeshData(UINT32 numVertices, UINT32 numIndices, VertexLayout layout, IndexType indexType)
    {
        SPtr<VertexDataDesc> vertexDesc = VertexLayoutVertexDesc(layout);
        _meshData = te_shared_ptr_new<MeshData>(numVertices, numIndices, vertexDesc, indexType);
    }

    RendererMeshData::RendererMeshData(const SPtr<MeshData>& meshData)
        : _meshData(meshData)
    { }

    void RendererMeshData::GetPositions(Vector3* buffer, UINT32 size)
    {
        if (!_meshData->GetVertexDesc()->HasElement(VES_POSITION))
            return;

        UINT32 numElements = _meshData->GetNumVertices();
        assert(numElements * sizeof(Vector3) == size);

        _meshData->GetVertexData(VES_POSITION, buffer, size);
    }

    void RendererMeshData::SetPositions(Vector3* buffer, UINT32 size)
    {
        if (!_meshData->GetVertexDesc()->HasElement(VES_POSITION))
            return;

        UINT32 numElements = _meshData->GetNumVertices();
        assert(numElements * sizeof(Vector3) == size);

        _meshData->SetVertexData(VES_POSITION, buffer, size);
    }

    void RendererMeshData::GetNormals(Vector3* buffer, UINT32 size)
    {
        if (!_meshData->GetVertexDesc()->HasElement(VES_NORMAL))
            return;

        UINT32 numElements = _meshData->GetNumVertices();
        assert(numElements * sizeof(Vector3) == size);

        _meshData->SetVertexData(VES_NORMAL, buffer, size);
    }

    void RendererMeshData::SetNormals(Vector3* buffer, UINT32 size)
    {
        if (!_meshData->GetVertexDesc()->HasElement(VES_NORMAL))
            return;

        UINT32 numElements = _meshData->GetNumVertices();
        assert(numElements * sizeof(Vector3) == size);

        _meshData->SetVertexData(VES_NORMAL, buffer, size);
    }

    void RendererMeshData::GetTangents(Vector4* buffer, UINT32 size)
    {
        if (!_meshData->GetVertexDesc()->HasElement(VES_TANGENT))
            return;

        UINT32 numElements = _meshData->GetNumVertices();
        assert(numElements * sizeof(Vector4) == size);

        _meshData->SetVertexData(VES_TANGENT, buffer, size);
    }

    void RendererMeshData::SetTangents(Vector4* buffer, UINT32 size)
    {
        if (!_meshData->GetVertexDesc()->HasElement(VES_TANGENT))
            return;

        UINT32 numElements = _meshData->GetNumVertices();
        assert(numElements * sizeof(Vector4) == size);

        _meshData->SetVertexData(VES_TANGENT, buffer, size);
    }

    void RendererMeshData::GetBiTangents(Vector4* buffer, UINT32 size)
    {
        if (!_meshData->GetVertexDesc()->HasElement(VES_BITANGENT))
            return;

        UINT32 numElements = _meshData->GetNumVertices();
        assert(numElements * sizeof(Vector4) == size);

        _meshData->SetVertexData(VES_BITANGENT, buffer, size);
    }

    void RendererMeshData::SetBiTangents(Vector4* buffer, UINT32 size)
    {
        if (!_meshData->GetVertexDesc()->HasElement(VES_BITANGENT))
            return;

        UINT32 numElements = _meshData->GetNumVertices();
        assert(numElements * sizeof(Vector4) == size);

        _meshData->SetVertexData(VES_BITANGENT, buffer, size);
    }

    void RendererMeshData::GetColors(Color* buffer, UINT32 size)
    {
        if (!_meshData->GetVertexDesc()->HasElement(VES_COLOR, 0))
            return;

        Vector4* colors = te_allocate<Vector4>(sizeof(Vector4) * _meshData->GetNumVertices());
        UINT32 numElements = _meshData->GetNumVertices();
        assert(numElements * sizeof(Vector4) == size);

        _meshData->GetVertexData(VES_COLOR, colors, size, 0);

        for (UINT32 i = 0; i < _meshData->GetNumVertices(); i++)
        {
           buffer[i].r = colors[i].x;
           buffer[i].g = colors[i].y;
           buffer[i].b = colors[i].z;
           buffer[i].a = colors[i].w;
        }

        te_delete(colors);
    }

    void RendererMeshData::GetColors(Vector4* buffer, UINT32 size)
    {
        if (!_meshData->GetVertexDesc()->HasElement(VES_COLOR))
            return;

        UINT32 numElements = _meshData->GetNumVertices();
        assert(numElements * sizeof(Vector4) == size);

        _meshData->SetVertexData(VES_COLOR, buffer, size);
    }

    void RendererMeshData::SetColors(Color* buffer, UINT32 size)
    {
        if (!_meshData->GetVertexDesc()->HasElement(VES_COLOR))
            return;

        Vector4* colors = te_allocate<Vector4>(sizeof(Vector4)* _meshData->GetNumVertices());
        for (UINT32 i = 0; i < _meshData->GetNumVertices(); i++)
        {
            colors[i] = Vector4(buffer[i].r, buffer[i].g, buffer[i].b, buffer[i].a);
        }

        UINT32 numElements = _meshData->GetNumVertices();
        assert(numElements * sizeof(Vector4) == size);

        _meshData->SetVertexData(VES_COLOR, colors, size);

        te_delete(colors);
    }

    void RendererMeshData::SetColors(Vector4* buffer, UINT32 size)
    {
        if (!_meshData->GetVertexDesc()->HasElement(VES_COLOR))
            return;

        UINT32 numElements = _meshData->GetNumVertices();
        assert(numElements * sizeof(Vector4) == size);

        _meshData->SetVertexData(VES_COLOR, buffer, size);
    }

    void RendererMeshData::GetUV0(Vector2* buffer, UINT32 size)
    {
        if (!_meshData->GetVertexDesc()->HasElement(VES_TEXCOORD, 0))
            return;

        UINT32 numElements = _meshData->GetNumVertices();
        assert(numElements * sizeof(Vector2) == size);

        _meshData->GetVertexData(VES_TEXCOORD, buffer, size, 0);
    }

    void RendererMeshData::SetUV0(Vector2* buffer, UINT32 size)
    {
        if (!_meshData->GetVertexDesc()->HasElement(VES_TEXCOORD, 0))
            return;

        UINT32 numElements = _meshData->GetNumVertices();
        assert(numElements * sizeof(Vector2) == size);

        _meshData->SetVertexData(VES_TEXCOORD, buffer, size, 0);
    }

    void RendererMeshData::GetUV1(Vector2* buffer, UINT32 size)
    {
        if (!_meshData->GetVertexDesc()->HasElement(VES_TEXCOORD, 1))
            return;

        UINT32 numElements = _meshData->GetNumVertices();
        assert(numElements * sizeof(Vector2) == size);

        _meshData->GetVertexData(VES_TEXCOORD, buffer, size, 1);
    }

    void RendererMeshData::SetUV1(Vector2* buffer, UINT32 size)
    {
        if (!_meshData->GetVertexDesc()->HasElement(VES_TEXCOORD, 1))
            return;

        UINT32 numElements = _meshData->GetNumVertices();
        assert(numElements * sizeof(Vector2) == size);

        _meshData->SetVertexData(VES_TEXCOORD, buffer, size, 1);
    }

    void RendererMeshData::GetBoneWeights(BoneWeight* buffer, UINT32 size)
    {
        SPtr<VertexDataDesc> vertexDesc = _meshData->GetVertexDesc();

        if (!vertexDesc->HasElement(VES_BLEND_WEIGHTS) ||
            !vertexDesc->HasElement(VES_BLEND_INDICES))
            return;

        UINT32 numElements = _meshData->GetNumVertices();
        assert(numElements * sizeof(BoneWeight) == size);

        UINT8* weightPtr = _meshData->GetElementData(VES_BLEND_WEIGHTS);
        UINT8* indexPtr = _meshData->GetElementData(VES_BLEND_INDICES);

        UINT32 stride = vertexDesc->GetVertexStride(0);

        BoneWeight* weightDst = buffer;
        for (UINT32 i = 0; i < numElements; i++)
        {
            UINT8* indices = (UINT8*)indexPtr;
            float* weights = (float*)weightPtr;

            weightDst->Index0 = indices[0];
            weightDst->Index1 = indices[1];
            weightDst->Index2 = indices[2];
            weightDst->Index3 = indices[3];

            weightDst->Weight0 = weights[0];
            weightDst->Weight1 = weights[1];
            weightDst->Weight2 = weights[2];
            weightDst->Weight3 = weights[3];

            weightDst++;
            indexPtr += stride;
            weightPtr += stride;
        }
    }

    void RendererMeshData::SetBoneWeights(BoneWeight* buffer, UINT32 size)
    {
        SPtr<VertexDataDesc> vertexDesc = _meshData->GetVertexDesc();

        if (!vertexDesc->HasElement(VES_BLEND_WEIGHTS) ||
            !vertexDesc->HasElement(VES_BLEND_INDICES))
            return;

        UINT32 numElements = _meshData->GetNumVertices();
        assert(numElements * sizeof(BoneWeight) == size);

        UINT8* weightPtr = _meshData->GetElementData(VES_BLEND_WEIGHTS);
        UINT8* indexPtr = _meshData->GetElementData(VES_BLEND_INDICES);

        UINT32 stride = vertexDesc->GetVertexStride(0);

        BoneWeight* weightSrc = buffer;
        for (UINT32 i = 0; i < numElements; i++)
        {
            UINT8* indices = (UINT8*)indexPtr;
            float* weights = (float*)weightPtr;

            indices[0] = static_cast<UINT8>(weightSrc->Index0);
            indices[1] = static_cast<UINT8>(weightSrc->Index1);
            indices[2] = static_cast<UINT8>(weightSrc->Index2);
            indices[3] = static_cast<UINT8>(weightSrc->Index3);

            weights[0] = weightSrc->Weight0;
            weights[1] = weightSrc->Weight1;
            weights[2] = weightSrc->Weight2;
            weights[3] = weightSrc->Weight3;

            weightSrc++;
            indexPtr += stride;
            weightPtr += stride;
        }
    }

    void RendererMeshData::GetIndices(UINT32* buffer, UINT32 size)
    {
        UINT32 indexSize = _meshData->GetIndexElementSize();
        UINT32 numIndices = _meshData->GetNumIndices();

        assert(numIndices * indexSize == size);

        if (_meshData->GetIndexType() == IT_16BIT)
        {
            UINT16* src = _meshData->GetIndices16();
            UINT32* dest = buffer;

            for (UINT32 i = 0; i < numIndices; i++)
            {
                *dest = *src;

                src++;
                dest++;
            }
        }
        else
        {
            memcpy(buffer, _meshData->GetIndices32(), size);
        }
    }

    void RendererMeshData::SetIndices(UINT32* buffer, UINT32 size)
    {
        UINT32 indexSize = _meshData->GetIndexElementSize();
        UINT32 numIndices = _meshData->GetNumIndices();

        assert(numIndices * indexSize == size);

        if (_meshData->GetIndexType() == IT_16BIT)
        {
            UINT16* dest = _meshData->GetIndices16();
            UINT32* src = buffer;

            for (UINT32 i = 0; i < numIndices; i++)
            {
                *dest = static_cast<UINT16>(*src);

                src++;
                dest++;
            }
        }
        else
        {
            memcpy(_meshData->GetIndices32(), buffer, size);
        }
    }

    SPtr<RendererMeshData> RendererMeshData::Create(UINT32 numVertices, UINT32 numIndices, VertexLayout layout, IndexType indexType)
    {
        return Renderer::CreateMeshData(numVertices, numIndices, layout, indexType);
    }

    SPtr<RendererMeshData> RendererMeshData::Create(const SPtr<MeshData>& meshData)
    {
        return Renderer::CreateMeshData(meshData);
    }

    SPtr<VertexDataDesc> RendererMeshData::VertexLayoutVertexDesc(VertexLayout type)
    {
        SPtr<VertexDataDesc> vertexDesc = VertexDataDesc::Create();

        INT32 intType = (INT32)type;

        if (intType == 0)
            type = VertexLayout::Position;

        if ((intType & (INT32)VertexLayout::Position) != 0)
            vertexDesc->AddVertElem(VET_FLOAT3, VES_POSITION);

        if ((intType & (INT32)VertexLayout::BoneWeights) != 0)
        {
            vertexDesc->AddVertElem(VET_UBYTE4, VES_BLEND_INDICES);
            vertexDesc->AddVertElem(VET_FLOAT4, VES_BLEND_WEIGHTS);
        }

        if ((intType & (INT32)VertexLayout::Normal) != 0)
            vertexDesc->AddVertElem(VET_FLOAT3, VES_NORMAL);

        if ((intType & (INT32)VertexLayout::Tangent) != 0)
            vertexDesc->AddVertElem(VET_FLOAT4, VES_TANGENT);

        if ((intType & (INT32)VertexLayout::BiTangent) != 0)
            vertexDesc->AddVertElem(VET_FLOAT4, VES_BITANGENT);

        if ((intType & (INT32)VertexLayout::UV0) != 0)
            vertexDesc->AddVertElem(VET_FLOAT2, VES_TEXCOORD, 0);

        if ((intType & (INT32)VertexLayout::UV1) != 0)
            vertexDesc->AddVertElem(VET_FLOAT2, VES_TEXCOORD, 1);

        if ((intType & (INT32)VertexLayout::Color) != 0)
            vertexDesc->AddVertElem(VET_FLOAT4, VES_COLOR);

        return vertexDesc;
    }
}
