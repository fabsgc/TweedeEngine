#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"

namespace te
{
    template<typename PluginFactory, class ...Args>
    class TE_CORE_EXPORT PluginManager : public Module<PluginManager<PluginFactory>>
    {
    public:
        TE_MODULE_STATIC_HEADER_MEMBER(PluginManager)

        /**
         * Initializes the physics manager and a particular physics implementation.
         * @param[in]	pluginName	Name of the plugin containing a plugin implementation.
         */
        PluginManager(const String& pluginName, Args &&...args)
            : _plugin(nullptr)
            , _factory(nullptr)
        {
            _plugin = DynLibManager::Instance().Load(pluginName);

            if (_plugin != nullptr)
            {
                typedef PluginFactory* (*LoadPluginFunc)();

                LoadPluginFunc loadPluginFunc = (LoadPluginFunc)_plugin->GetSymbol("LoadPlugin");
                _factory = loadPluginFunc();

                if (_factory != nullptr)
                    _factory->StartUp(std::forward<Args>(args)...);
            }
        }

        ~PluginManager()
        {
            if (_plugin != nullptr)
            {
                if (_factory != nullptr)
                {
                    typedef void(*UnloadPluginFunc)(PluginFactory*);

                    UnloadPluginFunc unloadPluginFunc = (UnloadPluginFunc)_plugin->GetSymbol("UnloadPlugin");

                    _factory->ShutDown();
                    unloadPluginFunc(_factory);
                }

                DynLibManager::Instance().Unload(_plugin);
            }
        }

    private:
        DynLib* _plugin;
        PluginFactory* _factory;
    };
}