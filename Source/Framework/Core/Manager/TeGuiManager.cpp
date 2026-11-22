#include "Manager/TeGuiManager.h"
#include "Gui/TeGuiAPI.h"
#include "Gui/TeGuiAPIFactory.h"
#include "Utility/TeDynLib.h"
#include "TeCoreApplication.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(GuiManager)

    GuiManager::GuiManager(const String& pluginName)
        : _pluginName(pluginName)
        , _pluginLibrary(nullptr)
    { }

    GuiManager::~GuiManager()
    { }

    void GuiManager::OnStartUp()
    {
        gCoreApplication().LoadPlugin(_pluginName, &_pluginLibrary);

        const char* name = "";
        if (_pluginLibrary != nullptr)
        {
            typedef const char* (*GetPluginNameFunc)();

            GetPluginNameFunc getPluginNameFunc = (GetPluginNameFunc)_pluginLibrary->GetSymbol("GetPluginName");
            if (getPluginNameFunc != nullptr)
                name = getPluginNameFunc();
        }

        for (const auto& factory : _availableFactories)
        {
            if (factory->Name() == name)
            {
                _gui = factory->Create();
                if (_gui != nullptr)
                {
                    _guiFactory = factory;
                    return;
                }
            }
        }

        TE_ASSERT_ERROR(false, "Cannot initialize GUI API. Factory for '" + _pluginName + "' cannot be found.");
    }

    void GuiManager::RegisterFactory(SPtr<GuiAPIFactory> factory)
    {
        assert(factory != nullptr);

        _availableFactories.push_back(factory);
    }

    void GuiManager::OnShutDown()
    {
        if (_gui != nullptr)
        {
            _gui->Destroy();
            _gui = nullptr;
        }

        if (_guiFactory != nullptr)
        {
            _guiFactory->Destroy();
            _guiFactory = nullptr;
        }

        _availableFactories.clear();

        if (_pluginLibrary != nullptr)
        {
            gCoreApplication().UnloadPlugin(_pluginLibrary);
            _pluginLibrary = nullptr;
        }
    }
}
