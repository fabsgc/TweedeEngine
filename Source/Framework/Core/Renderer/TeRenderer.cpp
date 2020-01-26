#include "Renderer/TeRenderer.h"
#include "RenderAPI/TeRenderAPI.h"
#include "Manager/TeRendererManager.h"

namespace te
{
    Renderer::Renderer()
    { }

    SPtr<RendererMeshData> Renderer::_createMeshData(UINT32 numVertices, UINT32 numIndices, VertexLayout layout, IndexType indexType)
    {
        return te_shared_ptr<RendererMeshData>(new (te_allocate<RendererMeshData>())
            RendererMeshData(numVertices, numIndices, layout, indexType));
    }

    SPtr<RendererMeshData> Renderer::_createMeshData(const SPtr<MeshData>& meshData)
    {
        return te_shared_ptr<RendererMeshData>(new (te_allocate<RendererMeshData>())
            RendererMeshData(meshData));
    }

    SPtr<Renderer> gRenderer()
    {
        return std::static_pointer_cast<Renderer>(RendererManager::Instance().GetRenderer());
    }
}
