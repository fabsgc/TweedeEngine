#include "Renderer/TeRendererMeshData.h"
#include "RenderAPI/TeVertexDataDesc.h"
#include "Math/TeVector2.h"
#include "Math/TeVector3.h"
#include "Math/TeVector4.h"
#include "Utility/TeColor.h"
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

        UINT8* normalSrc = _meshData->GetElementData(VES_NORMAL);
        UINT32 stride = _meshData->GetVertexDesc()->GetVertexStride(0);

        MeshUtility::UnpackNormals(normalSrc, buffer, numElements, stride);
    }

    void RendererMeshData::SetNormals(Vector3* buffer, UINT32 size)
    {
        if (!_meshData->GetVertexDesc()->HasElement(VES_NORMAL))
            return;

        UINT32 numElements = _meshData->GetNumVertices();
        assert(numElements * sizeof(Vector3) == size);

        UINT8* normalDst = _meshData->GetElementData(VES_NORMAL);
        UINT32 stride = _meshData->GetVertexDesc()->GetVertexStride(0);

        MeshUtility::PackNormals(buffer, normalDst, numElements, sizeof(Vector3), stride);
    }

    void RendererMeshData::GetTangents(Vector4* buffer, UINT32 size)
    {
        if (!_meshData->GetVertexDesc()->HasElement(VES_TANGENT))
            return;

        UINT32 numElements = _meshData->GetNumVertices();
        assert(numElements * sizeof(Vector4) == size);

        UINT8* tangentSrc = _meshData->GetElementData(VES_TANGENT);
        UINT32 stride = _meshData->GetVertexDesc()->GetVertexStride(0);

        MeshUtility::UnpackNormals(tangentSrc, buffer, numElements, stride);
    }

    void RendererMeshData::SetTangents(Vector4* buffer, UINT32 size)
    {
        if (!_meshData->GetVertexDesc()->HasElement(VES_TANGENT))
            return;

        UINT32 numElements = _meshData->GetNumVertices();
        assert(numElements * sizeof(Vector4) == size);

        UINT8* tangentDst = _meshData->GetElementData(VES_TANGENT);
        UINT32 stride = _meshData->GetVertexDesc()->GetVertexStride(0);

        MeshUtility::PackNormals(buffer, tangentDst, numElements, sizeof(Vector4), stride);
    }

    void RendererMeshData::GetBiTangents(Vector4* buffer, UINT32 size)
    {
        if (!_meshData->GetVertexDesc()->HasElement(VES_BITANGENT))
            return;

        UINT32 numElements = _meshData->GetNumVertices();
        assert(numElements * sizeof(Vector4) == size);

        UINT8* tangentSrc = _meshData->GetElementData(VES_BITANGENT);
        UINT32 stride = _meshData->GetVertexDesc()->GetVertexStride(0);

        MeshUtility::UnpackNormals(tangentSrc, buffer, numElements, stride);
    }

    void RendererMeshData::SetBiTangents(Vector4* buffer, UINT32 size)
    {
        if (!_meshData->GetVertexDesc()->HasElement(VES_BITANGENT))
            return;

        UINT32 numElements = _meshData->GetNumVertices();
        assert(numElements * sizeof(Vector4) == size);

        UINT8* tangentDst = _meshData->GetElementData(VES_BITANGENT);
        UINT32 stride = _meshData->GetVertexDesc()->GetVertexStride(0);

        MeshUtility::PackNormals(buffer, tangentDst, numElements, sizeof(Vector4), stride);
    }

    void RendererMeshData::GetColors(Color* buffer, UINT32 size)
    {
        if (!_meshData->GetVertexDesc()->HasElement(VES_COLOR))
            return;

        UINT32 numElements = _meshData->GetNumVertices();
        assert(numElements * sizeof(Vector4) == size);

        UINT8* colorSrc = _meshData->GetElementData(VES_COLOR);
        UINT32 stride = _meshData->GetVertexDesc()->GetVertexStride(0);

        Color* colorDst = buffer;
        for (UINT32 i = 0; i < numElements; i++)
        {
            PixelUtil::UnpackColor(colorDst, PF_RGBA8, (void*)colorSrc);

            colorSrc += stride;
            colorDst++;
        }
    }

    void RendererMeshData::SetColors(Color* buffer, UINT32 size)
    {
        if (!_meshData->GetVertexDesc()->HasElement(VES_COLOR))
            return;

        UINT32 numElements = _meshData->GetNumVertices();
        assert(numElements * sizeof(Vector4) == size);

        UINT8* colorDst = _meshData->GetElementData(VES_COLOR);
        UINT32 stride = _meshData->GetVertexDesc()->GetVertexStride(0);

        Color* colorSrc = buffer;
        for (UINT32 i = 0; i < numElements; i++)
        {
            PixelUtil::PackColor(*colorSrc, PF_RGBA8, (void*)colorDst);

            colorSrc++;
            colorDst += stride;
        }
    }

    void RendererMeshData::SetColors(UINT32* buffer, UINT32 size)
    {
        if (!_meshData->GetVertexDesc()->HasElement(VES_COLOR))
            return;

        UINT32 numElements = _meshData->GetNumVertices();
        assert(numElements * sizeof(UINT32) == size);

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
                *dest = *src;

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
        return RendererManager::Instance().GetRenderer()->_createMeshData(numVertices, numIndices, layout, indexType);
    }

    SPtr<RendererMeshData> RendererMeshData::Create(const SPtr<MeshData>& meshData)
    {
        return RendererManager::Instance().GetRenderer()->_createMeshData(meshData);
    }

    SPtr<VertexDataDesc> RendererMeshData::VertexLayoutVertexDesc(VertexLayout type)
    {
        SPtr<VertexDataDesc> vertexDesc = VertexDataDesc::Create();

        INT32 intType = (INT32)type;

        if (intType == 0)
            type = VertexLayout::Position;

        if ((intType & (INT32)VertexLayout::Position) != 0)
            vertexDesc->AddVertElem(VET_FLOAT3, VES_POSITION);

        if ((intType & (INT32)VertexLayout::Normal) != 0)
            vertexDesc->AddVertElem(VET_UBYTE4_NORM, VES_NORMAL);

        if ((intType & (INT32)VertexLayout::Tangent) != 0)
            vertexDesc->AddVertElem(VET_UBYTE4_NORM, VES_TANGENT);

        if ((intType & (INT32)VertexLayout::BiTangent) != 0)
            vertexDesc->AddVertElem(VET_UBYTE4_NORM, VES_BITANGENT);

        if ((intType & (INT32)VertexLayout::UV0) != 0)
            vertexDesc->AddVertElem(VET_FLOAT2, VES_TEXCOORD, 0);

        if ((intType & (INT32)VertexLayout::UV1) != 0)
            vertexDesc->AddVertElem(VET_FLOAT2, VES_TEXCOORD, 1);

        if ((intType & (INT32)VertexLayout::Color) != 0)
            vertexDesc->AddVertElem(VET_COLOR, VES_COLOR);

        return vertexDesc;
    }
}
