#include "Renderer/TeRenderer.h"
#include "RenderAPI/TeRenderAPI.h"
#include "Manager/TeRendererManager.h"

namespace te
{
    Renderer::Renderer()
    {
    }

    SPtr<Renderer> gRenderer()
    {
        return std::static_pointer_cast<Renderer>(RendererManager::Instance().GetRenderer());
    }
}