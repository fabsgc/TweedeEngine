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
        TE_MODULE_STATIC_HEADER_MEMBER(RendererManager)

        virtual ~RendererManager() = default;

        /** @copydoc Module::OnStartUp */
        void OnStartUp() override;

        /** @copydoc Module::OnShutDown */
        void OnShutDown() override;

        /** Initializes the renderer, making it ready to render. */
        SPtr<Renderer> Initialize(const String& pluginName, String rendererName, bool useAsDefault = true);

        /** Returns the current renderer. Null if no there is no renderer. */
        SPtr<Renderer> GetRenderer() { return _defaultRenderer; };

        /** 
         * Returns the renderer attached to the given name. 
         * Returns Null if renderer is not found or if no there is no renderer. 
         */
        SPtr<Renderer> GetRenderer(const String rendererName);

        /** Registers a new render API factory responsible for creating a specific render system type. */
        void RegisterFactory(SPtr<RendererFactory> factory);

    private:
        Vector<SPtr<RendererFactory>> _availableFactories;
        Map<String, SPtr<Renderer>> _renderers;
        SPtr<Renderer> _defaultRenderer;
    };
}
