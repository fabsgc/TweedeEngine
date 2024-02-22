#include "TeResourceImporterPrerequisites.h"
#include "Importer/TeImporter.h"
#include "TeResourceImporter.h"

namespace te
{
    /**	Returns a name of the plugin. */
    extern "C" TE_PLUGIN_EXPORT const char* GetPluginName()
    {
        static const char* pluginName = "ResourceImporter";
        return pluginName;
    }

    /**	Entry point to the plugin. Called by the engine when the plugin is loaded. */
    extern "C" TE_PLUGIN_EXPORT void* LoadPlugin()
    {
        ResourceImporter* importer = te_new<ResourceImporter>();
        Importer::Instance().RegisterAssetImporter(importer);
        return nullptr;
    }
}
