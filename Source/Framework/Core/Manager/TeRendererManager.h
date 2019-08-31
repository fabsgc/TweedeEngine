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
        /** Initializes the renderer, making it ready to render. */
        SPtr<Renderer> Initialize(const String& pluginFilename);

        /**	Returns the current renderer. Null if no renderer is active. */
        SPtr<Renderer> GetRenderer() { return _renderer; }

        /**	Registers a new render API factory responsible for creating a specific render system type. */
        void RegisterFactory(SPtr<RendererFactory> factory);

        TE_MODULE_STATIC_HEADER_MEMBER(RendererManager)
    private:
        Vector<SPtr<RendererFactory>> _availableFactories;
        SPtr<Renderer> _renderer;
    };
}
