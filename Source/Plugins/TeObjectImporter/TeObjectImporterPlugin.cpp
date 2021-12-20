#include "TeObjectImporterPrerequisites.h"
#include "TeObjectImporter.h"
#include "Importer/TeImporter.h"

namespace te
{
    /** Returns a name of the plugin. */
    extern "C" TE_PLUGIN_EXPORT const char* GetPluginName()
    {
        static const char* pluginName = "ObjectImporter";
        return pluginName;
    }

    /** Entry point to the plugin. Called by the engine when the plugin is loaded. */
    extern "C" TE_PLUGIN_EXPORT void* LoadPlugin()
    {
        ObjectImporter* importer = te_new<ObjectImporter>();
        Importer::Instance().RegisterAssetImporter(importer);
        return nullptr;
    }
}