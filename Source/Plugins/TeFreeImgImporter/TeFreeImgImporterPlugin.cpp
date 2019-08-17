#include "TeFreeImgImporterPrerequisites.h"
#include "TeFreeImgImporter.h"

namespace te
{
	/**	Returns a name of the plugin. */
	extern "C" TE_PLUGIN_EXPORT const char* GetPluginName()
	{
		static const char* pluginName = "FreeImgImporter";
		return pluginName;
	}

	/**	Entry point to the plugin. Called by the engine when the plugin is loaded. */
	extern "C" TE_PLUGIN_EXPORT void* LoadPlugin()
	{
		te_new<FreeImgImporter>();
		return nullptr;
	}
}