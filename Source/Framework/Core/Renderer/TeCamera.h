#pragma once

#include "TeCorePrerequisites.h"
#include "Scene/TeSceneActor.h"
#include "Renderer/TeViewport.h"
#include "CoreUtility/TeCoreObject.h"

namespace te
{
    /** Common base class for both sim and core thread implementations of Camera. */
	class TE_CORE_EXPORT CameraBase : public SceneActor
	{
	public:
        CameraBase() = default;
		virtual ~CameraBase() = default;

    protected:
        bool _main = false; /**< Determines does this camera render to the main render surface. */
    };

    /**
	 * Camera determines how is world geometry projected onto a 2D surface. You may position and orient it in space, set
	 * options like aspect ratio and field or view and it outputs view and projection matrices required for rendering.
	 */
	class TE_CORE_EXPORT Camera : public CameraBase, public CoreObject
	{
	public:
        ~Camera();

        /** @copydoc CoreObject::Initialize */
		void Initialize() override;

		/** @copydoc CoreObject::Destroy */
		void Destroy() override;

        /**
		 * Determines whether this is the main application camera. Main camera controls the final render surface that is
		 * displayed to the user.
		 */	
        void SetMain(bool main) { _main = main; };

		/** @copydoc setMain() */
		bool IsMain() const { return _main; }

        /**	Creates a new camera that renders to the specified portion of the provided render target. */
		static SPtr<Camera> Create();

        /**	Returns the viewport used by the camera. */
        const SPtr<Viewport> GetViewport() const { return _viewport; }

        void SetRenderTarget(SPtr<RenderTarget> renderTarget);

    protected:
        Camera(SPtr<RenderTarget> target = nullptr, float left = 0.0f, float top = 0.0f, float width = 1.0f, float height = 1.0f);
		Camera(const SPtr<Viewport>& viewport);

    protected:
		/** Viewport that describes a 2D rendering surface. */
		SPtr<Viewport> _viewport;
    };
}
