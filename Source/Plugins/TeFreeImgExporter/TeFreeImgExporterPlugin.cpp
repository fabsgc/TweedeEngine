#include "TeFreeImgExporterPrerequisites.h"
#include "Exporter/TeExporter.h"
#include "TeFreeImgExporter.h"

namespace te
{
    /**	Returns a name of the plugin. */
    extern "C" TE_PLUGIN_EXPORT const char* GetPluginName()
    {
        static const char* pluginName = "FreeImgExporter";
        return pluginName;
    }

    /**	Entry point to the plugin. Called by the engine when the plugin is loaded. */
    extern "C" TE_PLUGIN_EXPORT void* LoadPlugin()
    {
        FreeImgExporter* exporter = te_new<FreeImgExporter>();
        Exporter::Instance().RegisterAssetExporter(exporter);
        return nullptr;
    }
}
