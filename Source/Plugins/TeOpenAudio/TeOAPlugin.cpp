#include "TeOAPrerequisites.h"
#include "TeOAImporter.h"
#include "TeOAAudio.h"
#include "Audio/TeAudioManager.h"
#include "Importer/TeImporter.h"

namespace te
{
    class OAFactory : public AudioFactory
    {
    public:
        void StartUp() override
        {
            Audio::StartUp<OAAudio>();
        }

        void ShutDown() override
        {
            Audio::ShutDown();
        }
    };
    
    /**	Returns a name of the plugin. */
    extern "C" TE_PLUGIN_EXPORT const char* GetPluginName()
    {
        static const char* pluginName = "OpenAudio";
        return pluginName;
    }

    /**	Entry point to the plugin. Called by the engine when the plugin is loaded. */
    extern "C" TE_PLUGIN_EXPORT void* LoadPlugin()
    {
        OAImporter* importer = te_new<OAImporter>();
        Importer::Instance()._registerAssetImporter(importer);

        return te_new<OAFactory>();
    }

    /**	Exit point of the plugin. Called by the engine before the plugin is unloaded. */
    extern "C" TE_PLUGIN_EXPORT void UnloadPlugin(OAFactory* instance)
    {
        te_delete(instance);
    }
}
