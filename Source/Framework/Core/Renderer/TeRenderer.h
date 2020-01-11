#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    class TE_CORE_EXPORT Renderer
    {
    public:
        Renderer();
        virtual ~Renderer() = default;

        /** Initializes the renderer. Must be called before using the renderer. */
        virtual void Initialize() {};

        /**	Cleans up the renderer. Must be called before the renderer is deleted. */
        virtual void Destroy() { }

        /** Called every frame. Triggers render task callbacks. */
        virtual void Update() = 0;

        /** Name of the renderer. Used by materials to find an appropriate technique for this renderer. */
        virtual const String& GetName() const = 0;

        /** Called in order to render all currently active cameras. */
        virtual void RenderAll() = 0;

        /** Called whenever a new camera is created. */
        virtual void NotifyCameraAdded(Camera* camera) { }

        /**
         * Called whenever a camera's position or rotation is updated.
         *
         * @param[in]	camera		Camera that was updated.
         */
        virtual void NotifyCameraUpdated(Camera* camera) { }

        /** Called whenever a camera is destroyed. */
        virtual void NotifyCameraRemoved(Camera* camera) { }
    };

    /**	Provides easy access to Renderer. */
    SPtr<Renderer> TE_CORE_EXPORT gRenderer();

    /**	Set of options that can be used for controlling the renderer. */	
	struct TE_CORE_EXPORT RendererOptions
	{
		virtual ~RendererOptions() = default;
	};

    /**	Renderer information specific to a single render target. */
	struct TE_CORE_EXPORT RendererRenderTarget
	{
        SPtr<RenderTarget> target = nullptr;
        Camera* camera = nullptr;
	};
}
