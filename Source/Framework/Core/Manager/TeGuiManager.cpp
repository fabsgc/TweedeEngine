#include "Manager/TeGuiManager.h"
#include "Gui/TeGuiAPI.h"
#include "Gui/TeGuiAPIFactory.h"
#include "Utility/TeDynLib.h"
#include "Utility/TeDynLibManager.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(GuiAPIManager)

    GuiManager::GuiManager()
        : _guiInitialized(false)
    { }

    GuiManager::~GuiManager()
    {
        if (_gui != nullptr)
        {
            _gui->Destroy();
        }
    }

    SPtr<GuiAPI> GuiManager::Initialize(const String& pluginFilename)
    {
        DynLib* loadedLibrary = gDynLibManager().Load(pluginFilename);
        const char* name = "";

        if (loadedLibrary != nullptr)
        {
            typedef const char* (*GetPluginNameFunc)();

            GetPluginNameFunc getPluginNameFunc = (GetPluginNameFunc)loadedLibrary->GetSymbol("GetPluginName");
            name = getPluginNameFunc();
        }

        for (auto iter = _availableFactories.begin(); iter != _availableFactories.end(); ++iter)
        {
            if ((*iter)->Name() == name)
            {
                SPtr<GuiAPI> newGuiAPI = (*iter)->Create();
                _guiInitialized = true;

                if (newGuiAPI != nullptr)
                {
                    if (_gui != nullptr)
                    {
                        _gui->Destroy();
                        te_delete(&_gui);
                    }

                    _gui = newGuiAPI;
                    return _gui;
                }
            }
        }

        TE_ASSERT_ERROR(_gui != nullptr, "Cannot initialize renderer. Renderer with the name '" + pluginFilename + "' cannot be found.");

        return nullptr;
    }

    void GuiManager::RegisterFactory(SPtr<GuiAPIFactory> factory)
    {
        assert(factory != nullptr);

        _availableFactories.push_back(factory);
    }
}
