#include "Renderer/TeRenderer.h"
#include "Manager/TeRendererManager.h"

namespace te
{
    Renderer::Renderer()
    { }

    SPtr<RendererMeshData> Renderer::CreateMeshData(UINT32 numVertices, UINT32 numIndices, VertexLayout layout, IndexType indexType)
    {
        return te_shared_ptr<RendererMeshData>(new (te_allocate<RendererMeshData>())
            RendererMeshData(numVertices, numIndices, layout, indexType));
    }

    SPtr<RendererMeshData> Renderer::CreateMeshData(const SPtr<MeshData>& meshData)
    {
        return te_shared_ptr<RendererMeshData>(new (te_allocate<RendererMeshData>())
            RendererMeshData(meshData));
    }

    SPtr<Renderer> gRenderer()
    {
        return RendererManager::Instance().GetRenderer();
    }
}
