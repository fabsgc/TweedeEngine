#include "Physics/TePhysicsManager.h"
#include "Utility/TeDynLibManager.h"
#include "Utility/TeDynLib.h"

namespace te
{
    PhysicsManager::PhysicsManager(const String& pluginName)
        : _plugin(nullptr)
        , _factory(nullptr)
    {
        _plugin = DynLibManager::Instance().Load(pluginName);

        if(_plugin != nullptr)
        {
            typedef PhysicsFactory* (*LoadPluginFunc)();

            LoadPluginFunc loadPluginFunc = (LoadPluginFunc)_plugin->GetSymbol("LoadPlugin");
            _factory = loadPluginFunc();

            if (_factory != nullptr)
                _factory->StartUp();
        }
    }

    PhysicsManager::~PhysicsManager()
    {
        if (_plugin != nullptr)
        {
            if (_factory != nullptr)
            {
                typedef void (*UnloadPluginFunc)(PhysicsFactory*);

                UnloadPluginFunc unloadPluginFunc = (UnloadPluginFunc)_plugin->GetSymbol("UnloadPlugin");

                _factory->ShutDown();
                unloadPluginFunc(_factory);
            }

            DynLibManager::Instance().Unload(_plugin);
        }
    }
}
