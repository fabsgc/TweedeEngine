#include "TeProjectImporterPrerequisites.h"
#include "Importer/TeImporter.h"
#include "TeProjectImporter.h"

namespace te
{
    /**	Returns a name of the plugin. */
    extern "C" TE_PLUGIN_EXPORT const char* GetPluginName()
    {
        static const char* pluginName = "ProjectImporter";
        return pluginName;
    }

    /**	Entry point to the plugin. Called by the engine when the plugin is loaded. */
    extern "C" TE_PLUGIN_EXPORT void* LoadPlugin()
    {
        ProjectImporter* importer = te_new<ProjectImporter>();
        Importer::Instance().RegisterAssetImporter(importer);
        return nullptr;
    }
}
