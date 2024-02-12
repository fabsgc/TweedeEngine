#include "TeSceneExporterPrerequisites.h"
#include "Exporter/TeExporter.h"
#include "TeSceneExporter.h"

namespace te
{
    /**	Returns a name of the plugin. */
    extern "C" TE_PLUGIN_EXPORT const char* GetPluginName()
    {
        static const char* pluginName = "SceneExporter";
        return pluginName;
    }

    /**	Entry point to the plugin. Called by the engine when the plugin is loaded. */
    extern "C" TE_PLUGIN_EXPORT void* LoadPlugin()
    {
        SceneExporter* exporter = te_new<SceneExporter>();
        Exporter::Instance().RegisterAssetExporter(exporter);
        return nullptr;
    }
}
