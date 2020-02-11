#include "TeRendererUtility.h"
#include "RenderAPI/TeVertexDataDesc.h"
#include "RenderAPI/TeRenderAPI.h"
#include "Material/TeMaterial.h"
#include "Material/TePass.h"
#include "Mesh/TeMesh.h"

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
			vbDesc.NumVerts = 4 * NUM_QUAD_VB_SLOTS;
			vbDesc.Usage = GBU_DYNAMIC;

			_fullScreenQuadVB = VertexBuffer::Create(vbDesc);

			UINT32 indices[] { 0, 1, 2, 1, 3, 2 };
			_fullScreenQuadIB->WriteData(0, sizeof(indices), indices, BWT_DISCARD);
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

        if (numInstances > 0)
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
        Vector3 vertices[4];

        if (rapiConventions.NDC_YAxis == Conventions::Axis::Down)
        {
            vertices[0] = Vector3(-1.0f, -1.0f, 0.0f);
            vertices[1] = Vector3(1.0f, -1.0f, 0.0f);
            vertices[2] = Vector3(-1.0f, 1.0f, 0.0f);
            vertices[3] = Vector3(1.0f, 1.0f, 0.0f);
        }
        else
        {
            vertices[0] = Vector3(-1.0f, 1.0f, 0.0f);
            vertices[1] = Vector3(1.0f, 1.0f, 0.0f);
            vertices[2] = Vector3(-1.0f, -1.0f, 0.0f);
            vertices[3] = Vector3(1.0f, -1.0f, 0.0f);
        }

        Vector2 uvs[4];
        if ((rapiConventions.UV_YAxis == Conventions::Axis::Up) ^ flipUV)
        {
            uvs[0] = Vector2(uv.x, uv.y + uv.height);
            uvs[1] = Vector2(uv.x + uv.width, uv.y + uv.height);
            uvs[2] = Vector2(uv.x, uv.y);
            uvs[3] = Vector2(uv.x + uv.width, uv.y);
        }
        else
        {
            uvs[0] = Vector2(uv.x, uv.y);
            uvs[1] = Vector2(uv.x + uv.width, uv.y);
            uvs[2] = Vector2(uv.x, uv.y + uv.height);
            uvs[3] = Vector2(uv.x + uv.width, uv.y + uv.height);
        }

        for (int i = 0; i < 4; i++)
        {
            uvs[i].x /= (float)textureSize.x;
            uvs[i].y /= (float)textureSize.y;
        }

        SPtr<MeshData> meshData = te_shared_ptr_new<MeshData>(4, 6, _fullscreenQuadVDesc);

        UINT8* vecIter = meshData->GetElementData(VES_POSITION);
        for (UINT32 i = 0; i < 4; i++)
        {
            memcpy(vecIter, &vertices[i], sizeof(vertices[i]));
        }

        UINT8* uvIter = meshData->GetElementData(VES_TEXCOORD);
        for (UINT32 i = 0; i < 4; i++)
        {
            memcpy(uvIter, &uvs[i], sizeof(uvs[i]));
        }

        UINT32 bufferSize = meshData->GetStreamSize(0);
        UINT8* srcVertBufferData = meshData->GetStreamData(0);

        void* dstData = _fullScreenQuadVB->Lock(_nextQuadVBSlot * bufferSize, bufferSize, GBL_WRITE_ONLY_NO_OVERWRITE);
        memcpy(dstData, srcVertBufferData, bufferSize);
        _fullScreenQuadVB->Unlock();

        RenderAPI& rapi = RenderAPI::Instance();

        rapi.SetVertexDeclaration(_fullscreenQuadVDecl);
        rapi.SetVertexBuffers(0, &_fullScreenQuadVB, 1);
        rapi.SetIndexBuffer(_fullScreenQuadIB);
        rapi.SetDrawOperation(DOT_TRIANGLE_LIST);
        rapi.DrawIndexed(0, 6, _nextQuadVBSlot * 4, 4, numInstances);

        _nextQuadVBSlot = (_nextQuadVBSlot + 1) % NUM_QUAD_VB_SLOTS;
    }

    RendererUtility& gRendererUtility()
    {
        return RendererUtility::Instance();
    }
}
