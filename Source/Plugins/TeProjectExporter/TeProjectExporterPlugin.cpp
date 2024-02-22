#include "TeProjectExporterPrerequisites.h"
#include "Exporter/TeExporter.h"
#include "TeProjectExporter.h"

namespace te
{
    /**	Returns a name of the plugin. */
    extern "C" TE_PLUGIN_EXPORT const char* GetPluginName()
    {
        static const char* pluginName = "ProjectExporter";
        return pluginName;
    }

    /**	Entry point to the plugin. Called by the engine when the plugin is loaded. */
    extern "C" TE_PLUGIN_EXPORT void* LoadPlugin()
    {
        ProjectExporter* exporter = te_new<ProjectExporter>();
        Exporter::Instance().RegisterAssetExporter(exporter);
        return nullptr;
    }
}
