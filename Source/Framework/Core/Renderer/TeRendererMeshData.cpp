#include "Renderer/TeRendererMeshData.h"
#include "RenderAPI/TeVertexDataDesc.h"
#include "Math/TeVector2.h"
#include "Math/TeVector3.h"
#include "Math/TeVector4.h"
#include "Utility/TeColor.h"
#include "Image/TePixelUtil.h"
#include "Manager/TeRendererManager.h"
#include "Renderer/TeRenderer.h"

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

        if ((intType & (INT32)VertexLayout::UV0) != 0)
            vertexDesc->AddVertElem(VET_FLOAT2, VES_TEXCOORD, 0);

        if ((intType & (INT32)VertexLayout::UV1) != 0)
            vertexDesc->AddVertElem(VET_FLOAT2, VES_TEXCOORD, 1);

        if ((intType & (INT32)VertexLayout::Color) != 0)
            vertexDesc->AddVertElem(VET_COLOR, VES_COLOR);

        return vertexDesc;
    }
}
