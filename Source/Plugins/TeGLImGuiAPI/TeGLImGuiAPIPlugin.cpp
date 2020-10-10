#include "TeGLImGuiAPIPrerequisites.h"
#include "TeGLImGuiAPIFactory.h"
#include "Manager/TeGuiManager.h"

namespace te
{
    /** Returns a name of the plugin. */
    extern "C" TE_PLUGIN_EXPORT const char* GetPluginName()
    {
        return GLImGuiAPIFactory::SystemName;
    }

    /** Entry point to the plugin. Called by the engine when the plugin is loaded. */
    extern "C" TE_PLUGIN_EXPORT void* LoadPlugin()
    {
        GuiManager::Instance().RegisterFactory(te_shared_ptr_new<GLImGuiAPIFactory>());
        return nullptr;
    }
}