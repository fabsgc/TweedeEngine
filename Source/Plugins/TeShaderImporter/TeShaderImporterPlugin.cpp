#include "TeShaderImporterPrerequisites.h"
#include "TeShaderImporter.h"
#include "Importer/TeImporter.h"

namespace te
{
	/**	Returns a name of the plugin. */
	extern "C" TE_PLUGIN_EXPORT const char* GetPluginName()
	{
		static const char* pluginName = "ShaderImporter";
		return pluginName;
	}

	/**	Entry point to the plugin. Called by the engine when the plugin is loaded. */
	extern "C" TE_PLUGIN_EXPORT void* LoadPlugin()
	{
		ShaderImporter* importer = te_new<ShaderImporter>();
        Importer::Instance()._registerAssetImporter(importer);
		return nullptr;
	}
}