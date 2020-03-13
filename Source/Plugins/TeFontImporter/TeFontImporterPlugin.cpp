#include "TeFontImporterPrerequisites.h"
#include "Importer/TeImporter.h"
#include "TeFontImporter.h"

namespace te
{
    /**	Returns a name of the plugin. */
    extern "C" TE_PLUGIN_EXPORT const char* GetPluginName()
    {
        static const char* pluginName = "FontImporter";
        return pluginName;
    }

    /**	Entry point to the plugin. Called by the engine when the plugin is loaded. */
    extern "C" TE_PLUGIN_EXPORT void* LoadPlugin()
    {
        FontImporter* importer = te_new<FontImporter>();
        Importer::Instance()._registerAssetImporter(importer);
        return nullptr;
    }
}
