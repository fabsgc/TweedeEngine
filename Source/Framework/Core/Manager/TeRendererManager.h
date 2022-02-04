#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"
#include "Renderer/TeRenderer.h"
#include "Renderer/TeRendererFactory.h"

namespace te
{
    /** Manager that handles render system start up. */
    class TE_CORE_EXPORT RendererManager : public Module<RendererManager>
    {
    public:
        virtual ~RendererManager();

        TE_MODULE_STATIC_HEADER_MEMBER(RendererManager)

        /** Initializes the renderer, making it ready to render. */
        SPtr<Renderer> Initialize(const String& pluginName, String rendererName, bool useAsDefault = true);

        /** Returns the current renderer. Null if no renderer is active. */
        SPtr<Renderer> GetRenderer() { return _defaultRenderer; }

        /** Registers a new render API factory responsible for creating a specific render system type. */
        void RegisterFactory(SPtr<RendererFactory> factory);

    private:
        Vector<SPtr<RendererFactory>> _availableFactories;
        Map<String, SPtr<Renderer>> _renderers;
        SPtr<Renderer> _defaultRenderer;
    };
}
