
#include "TeRenderManPrerequisites.h"
#include "TeRenderManFactory.h"
#include "TeRenderManIBLUtility.h"
#include "TeRendererTextures.h"
#include "Manager/TeRendererManager.h"

namespace te
{
    /** Returns a name of the plugin. */
    extern "C" TE_PLUGIN_EXPORT const char* GetPluginName()
    {
        return RenderManFactory::SystemName;
    }

    /** Entry point to the plugin. Called by the engine when the plugin is loaded. */
    extern "C" TE_PLUGIN_EXPORT void* LoadPlugin()
    {
        RendererManager::Instance().RegisterFactory(te_shared_ptr_new<RenderManFactory>());

        IBLUtility::StartUp<RenderManIBLUtility>();
        RendererTextures::StartUp();

        return nullptr;
    }

    extern "C" TE_PLUGIN_EXPORT void UnloadPlugin(RenderManFactory* instance)
    {
        IBLUtility::ShutDown();
        RendererTextures::ShutDown();
    }
}
