#include "TeGLRenderAPIPrerequisites.h"
#include "TeGLRenderAPIFactory.h"
#include "Manager/TeRenderAPIManager.h"

namespace te
{
    /**	Returns a name of the plugin. */
    extern "C" TE_PLUGIN_EXPORT const char* GetPluginName()
    {
        return GLRenderAPIFactory::SystemName;
    }

    /**	Entry point to the plugin. Called by the engine when the plugin is loaded. */
    extern "C" TE_PLUGIN_EXPORT void* LoadPlugin()
    {
        RenderAPIManager::Instance().RegisterFactory(te_shared_ptr_new<GLRenderAPIFactory>());
        return nullptr;
    }
}