#include "Audio/TeAudioManager.h"
#include "Utility/TeDynLibManager.h"
#include "Utility/TeDynLib.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(AudioManager)

    AudioManager::AudioManager(const String& pluginName)
    {
        _plugin = DynLibManager::Instance().Load(pluginName);

        if(_plugin != nullptr)
        {
            typedef AudioFactory* (*LoadPluginFunc)();

            LoadPluginFunc loadPluginFunc = (LoadPluginFunc)_plugin->GetSymbol("LoadPlugin");
            _factory = loadPluginFunc();

            if (_factory != nullptr)
                _factory->StartUp();
        }
    }

    AudioManager::~AudioManager()
    {
        if (_plugin != nullptr)
        {
            if (_factory != nullptr)
            {
                typedef void (*UnloadPluginFunc)(AudioFactory*);

                UnloadPluginFunc unloadPluginFunc = (UnloadPluginFunc)_plugin->GetSymbol("UnloadPlugin");

                _factory->ShutDown();
                unloadPluginFunc(_factory);
            }

            DynLibManager::Instance().Unload(_plugin);
        }
    }
}
