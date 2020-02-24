#include "TeRendererUtility.h"
#include "Renderer/TeBlitMat.h"
#include "RenderAPI/TeVertexDataDesc.h"
#include "RenderAPI/TeRenderAPI.h"
#include "Material/TeMaterial.h"
#include "Material/TePass.h"
#include "Mesh/TeMesh.h"
#include "Mesh/TeShapeMeshes3D.h"

namespace te
{
    RendererUtility::RendererUtility()
    {
        {
            _fullscreenQuadVDesc = te_shared_ptr_new<VertexDataDesc>();
            _fullscreenQuadVDesc->AddVertElem(VET_FLOAT3, VES_POSITION);
            _fullscreenQuadVDesc->AddVertElem(VET_FLOAT2, VES_TEXCOORD);

            INDEX_BUFFER_DESC ibDesc;
            ibDesc.Type = IT_32BIT;
            ibDesc.NumIndices = 6;
            ibDesc.Usage = GBU_DYNAMIC;

            _fullScreenQuadIB = IndexBuffer::Create(ibDesc);
            _fullscreenQuadVDecl = VertexDeclaration::Create(_fullscreenQuadVDesc);

            VERTEX_BUFFER_DESC vbDesc;
            vbDesc.VertexSize = _fullscreenQuadVDecl->GetProperties().GetVertexSize(0);
            vbDesc.NumVerts = NUM_QUAD_VB_SLOTS * 4;
            vbDesc.Usage = GBU_DYNAMIC;

            _fullScreenQuadVB = VertexBuffer::Create(vbDesc);

            UINT32 indices[] { 0, 1, 2, 1, 3, 2 };
            _fullScreenQuadIB->WriteData(0, sizeof(indices), indices, BWT_DISCARD);
        }

        {
            SPtr<VertexDataDesc> vertexDesc = te_shared_ptr_new<VertexDataDesc>();
            vertexDesc->AddVertElem(VET_FLOAT3, VES_POSITION);

            UINT32 numVertices = 0;
            UINT32 numIndices = 0;

            ShapeMeshes3D::GetNumElementsAABox(numVertices, numIndices);
            SPtr<MeshData> meshData = te_shared_ptr_new<MeshData>(numVertices, numIndices, vertexDesc);

            UINT32* indexData = meshData->GetIndices32();
            UINT8* positionData = meshData->GetElementData(VES_POSITION);

            AABox localBox(-Vector3::ONE * 500.0f, Vector3::ONE * 500.0f);
            ShapeMeshes3D::SolidAABox(localBox, positionData, nullptr, nullptr, 0,
                vertexDesc->GetVertexStride(), indexData, 0);

            _skyBoxMesh = Mesh::_createPtr(meshData);
        }
    }

    RendererUtility::~RendererUtility()
    { }

    void RendererUtility::SetPass(const SPtr<Material>& material, UINT32 passIdx, UINT32 techniqueIdx)
    {
        RenderAPI& rapi = RenderAPI::Instance();
        SPtr<Pass> pass = material->GetPass(passIdx, techniqueIdx);
        rapi.SetGraphicsPipeline(pass->GetGraphicsPipelineState());
        rapi.SetStencilRef(pass->GetStencilRefValue());
    }

    void RendererUtility::SetComputePass(const SPtr<Material>& material, UINT32 passIdx)
    {
        RenderAPI& rapi = RenderAPI::Instance();
        SPtr<Pass> pass = material->GetPass(passIdx);
        rapi.SetComputePipeline(pass->GetComputePipelineState());
    }

    void RendererUtility::SetPassParams(const SPtr<GpuParams> gpuParams, UINT32 gpuParamsBindFlags, bool isInstanced)
    {
        if (gpuParams == nullptr)
            return;

        RenderAPI& rapi = RenderAPI::Instance();

        if(isInstanced)
            rapi.SetGpuParams(gpuParams, gpuParamsBindFlags, GPU_BIND_PARAM_BLOCK_ALL_EXCEPT, { "PerCameraBuffer"});
        else
            rapi.SetGpuParams(gpuParams, gpuParamsBindFlags, GPU_BIND_PARAM_BLOCK_ALL_EXCEPT, { "PerCameraBuffer", "PerInstanceBuffer"});
    }

    void RendererUtility::Draw(const SPtr<Mesh>& mesh, UINT32 numInstances)
    {
        Draw(mesh, mesh->GetProperties().GetSubMesh(0), numInstances);
    }

    void RendererUtility::Draw(const SPtr<Mesh>& mesh, const SubMesh& subMesh, UINT32 numInstances)
    {
        RenderAPI& rapi = RenderAPI::Instance();
        SPtr<VertexData> vertexData = mesh->GetVertexData();

        rapi.SetVertexDeclaration(mesh->GetVertexData()->vertexDeclaration);

        auto& vertexBuffers = vertexData->GetBuffers();
        if (vertexBuffers.size() > 0)
        {
            SPtr<VertexBuffer> buffers[TE_MAX_BOUND_VERTEX_BUFFERS];

            UINT32 endSlot = 0;
            UINT32 startSlot = TE_MAX_BOUND_VERTEX_BUFFERS;
            for (auto iter = vertexBuffers.begin(); iter != vertexBuffers.end(); ++iter)
            {
                if (iter->first >= TE_MAX_BOUND_VERTEX_BUFFERS)
                    TE_ASSERT_ERROR(false, "Buffer index out of range", __FILE__, __LINE__);

                startSlot = std::min(iter->first, startSlot);
                endSlot = std::max(iter->first, endSlot);
            }

            for (auto iter = vertexBuffers.begin(); iter != vertexBuffers.end(); ++iter)
            {
                buffers[iter->first - startSlot] = iter->second;
            }

            rapi.SetVertexBuffers(startSlot, buffers, endSlot - startSlot + 1);
        }

        SPtr<IndexBuffer> indexBuffer = mesh->GetIndexBuffer();
        rapi.SetIndexBuffer(indexBuffer);

        rapi.SetDrawOperation(subMesh.DrawOp);

        UINT32 indexCount = subMesh.IndexCount;

        if (numInstances > 1)
        {
            rapi.DrawIndexed(subMesh.IndexOffset + mesh->GetIndexOffset(), indexCount, mesh->GetVertexOffset(),
                vertexData->vertexCount, numInstances);
        }
        else
        {
            rapi.DrawIndexed(subMesh.IndexOffset + mesh->GetIndexOffset(), indexCount, mesh->GetVertexOffset(),
                vertexData->vertexCount, 0);
        }

        mesh->_notifyUsedOnGPU();
    }

    void RendererUtility::DrawScreenQuad(const Rect2& uv, const Vector2I& textureSize, UINT32 numInstances, bool flipUV)
    {
        // Note: Consider drawing the quad using a single large triangle for possibly better performance
        // Note2: Consider setting quad size in shader instead of rebuilding the mesh every time
        const Conventions& rapiConventions = gCaps().Convention;

        struct VertexBuffer
        {
            Vector3 Position;
            Vector2 Texture;
        };

        VertexBuffer* dstData = (VertexBuffer *)_fullScreenQuadVB->Lock(
            _nextQuadVBSlot * sizeof(VertexBuffer) * 4, 
            sizeof(VertexBuffer) * 4, GBL_WRITE_ONLY_NO_OVERWRITE);

        if (rapiConventions.NDC_YAxis == Conventions::Axis::Down)
        {
            dstData[0].Position = Vector3(-1.0f, -1.0f, 0.0f);
            dstData[1].Position = Vector3(1.0f, -1.0f, 0.0f);
            dstData[2].Position = Vector3(-1.0f, 1.0f, 0.0f);
            dstData[3].Position = Vector3(1.0f, 1.0f, 0.0f);
        }
        else
        {
            dstData[0].Position = Vector3(-1.0f, 1.0f, 0.0f);
            dstData[1].Position = Vector3(1.0f, 1.0f, 0.0f);
            dstData[2].Position = Vector3(-1.0f, -1.0f, 0.0f);
            dstData[3].Position = Vector3(1.0f, -1.0f, 0.0f);
        }

        if ((rapiConventions.UV_YAxis == Conventions::Axis::Up) ^ flipUV)
        {
            dstData[0].Texture = Vector2(uv.x, uv.y + uv.height);
            dstData[1].Texture = Vector2(uv.x + uv.width, uv.y + uv.height);
            dstData[2].Texture = Vector2(uv.x, uv.y);
            dstData[3].Texture = Vector2(uv.x + uv.width, uv.y);
        }
        else
        {
            dstData[0].Texture = Vector2(uv.x, uv.y);
            dstData[1].Texture = Vector2(uv.x + uv.width, uv.y);
            dstData[2].Texture = Vector2(uv.x, uv.y + uv.height);
            dstData[3].Texture = Vector2(uv.x + uv.width, uv.y + uv.height);
        }

        for (int i = 0; i < 4; i++)
        {
            dstData[i].Texture.x /= (float)textureSize.x;
            dstData[i].Texture.y /= (float)textureSize.y;
        }

        _fullScreenQuadVB->Unlock();

        RenderAPI& rapi = RenderAPI::Instance();

        rapi.SetVertexDeclaration(_fullscreenQuadVDecl);
        rapi.SetVertexBuffers(0, &_fullScreenQuadVB, 1);
        rapi.SetIndexBuffer(_fullScreenQuadIB);
        rapi.SetDrawOperation(DOT_TRIANGLE_LIST);
        rapi.DrawIndexed(0, 6, _nextQuadVBSlot * 4, 4, numInstances);

        _nextQuadVBSlot = (_nextQuadVBSlot + 1) % NUM_QUAD_VB_SLOTS;
        _nextQuadVBSlot = 0;
    }

    void RendererUtility::Blit(const SPtr<Texture>& texture, const Rect2I& area, bool flipUV, bool isDepth, bool isFiltered)
    {
        auto& texProps = texture->GetProperties();

        Rect2 fArea((float)area.x, (float)area.y, (float)area.width, (float)area.height);
        if (area.width == 0 || area.height == 0)
        {
            fArea.x = 0.0f;
            fArea.y = 0.0f;
            fArea.width = (float)texProps.GetWidth();
            fArea.height = (float)texProps.GetHeight();
        }

        if (texProps.GetNumSamples() == 1) // RenderTarget without MSAA need Normalized device coordinates
        {
            fArea.width = 1.0f;
            fArea.height = 1.0f;
        }

        BlitMat* blitMat = BlitMat::Get();
        blitMat->Execute(texture, fArea, flipUV, texProps.GetNumSamples(), isDepth);
    }

    RendererUtility& gRendererUtility()
    {
        return RendererUtility::Instance();
    }
}
