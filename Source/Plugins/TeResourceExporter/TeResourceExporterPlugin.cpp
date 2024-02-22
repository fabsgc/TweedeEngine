#include "TeResourceExporterPrerequisites.h"
#include "Exporter/TeExporter.h"
#include "TeResourceExporter.h"

namespace te
{
    /**	Returns a name of the plugin. */
    extern "C" TE_PLUGIN_EXPORT const char* GetPluginName()
    {
        static const char* pluginName = "ResourceExporter";
        return pluginName;
    }

    /**	Entry point to the plugin. Called by the engine when the plugin is loaded. */
    extern "C" TE_PLUGIN_EXPORT void* LoadPlugin()
    {
        ResourceExporter* exporter = te_new<ResourceExporter>();
        Exporter::Instance().RegisterAssetExporter(exporter);
        return nullptr;
    }
}
