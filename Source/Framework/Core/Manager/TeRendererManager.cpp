#include "Manager/TeRendererManager.h"
#include "Renderer/TeRenderer.h"
#include "Renderer/TeRendererFactory.h"
#include "Utility/TeDynLib.h"
#include "Utility/TeDynLibManager.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(RendererManager)

    RendererManager::~RendererManager()
    {
        for (auto& renderer : _renderers)
        {
            renderer.second->Destroy();
        }
    }

    SPtr<Renderer> RendererManager::Initialize(const String& pluginName, String rendererName, bool useAsDefault)
    {
        DynLib* loadedLibrary = gDynLibManager().Load(pluginName);
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
                SPtr<Renderer> renderer = (*iter)->Create();
                if (renderer != nullptr)
                {
                    renderer->Initialize();

                    auto it = _renderers.find(rendererName);
                    if (it != _renderers.end())
                        it->second->Destroy();

                    if (useAsDefault)
                        _defaultRenderer = renderer;

                    _renderers[rendererName] = renderer;
                    return renderer;
                }
            }
        }

        TE_ASSERT_ERROR(_renderers[name] != nullptr, "Cannot initialize renderer. Renderer with the name '" + pluginName + "' cannot be found.");

        return nullptr;
    }

    void RendererManager::RegisterFactory(SPtr<RendererFactory> factory)
    {
        assert(factory != nullptr);

        _availableFactories.push_back(factory);
    }

    SPtr<Renderer> RendererManager::GetRenderer(const String rendererName = "")
    {
        auto it = _renderers.find(rendererName);
        if(it != _renderers.end())
            return it->second;

        return nullptr;
    }
}
