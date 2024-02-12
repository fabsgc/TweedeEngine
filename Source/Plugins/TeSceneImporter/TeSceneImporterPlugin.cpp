#include "TeSceneImporterPrerequisites.h"
#include "Importer/TeImporter.h"
#include "TeSceneImporter.h"

namespace te
{
    /**	Returns a name of the plugin. */
    extern "C" TE_PLUGIN_EXPORT const char* GetPluginName()
    {
        static const char* pluginName = "SceneImporter";
        return pluginName;
    }

    /**	Entry point to the plugin. Called by the engine when the plugin is loaded. */
    extern "C" TE_PLUGIN_EXPORT void* LoadPlugin()
    {
        SceneImporter* importer = te_new<SceneImporter>();
        Importer::Instance().RegisterAssetImporter(importer);
        return nullptr;
    }
}
