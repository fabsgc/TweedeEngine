#pragma once

#include "TeCorePrerequisites.h"
#include "RenderAPI/TeVideoMode.h"
#include "Utility/TeEvent.h"

namespace te
{
    /** Contains various properties that describe a render target. */
	class TE_CORE_EXPORT RenderTargetProperties
	{
	public:
		virtual ~RenderTargetProperties() = default;

		/** Width of the render target, in pixels. */
		UINT32 Width = 0;

		/** Height of the render target, in pixels. */
		UINT32 Height = 0;

		/**
		 * True if the render target will wait for vertical sync before swapping buffers. This will eliminate
		 * tearing but may increase input latency.
		 */
		bool VSync = false;

		/** True if pixels written to the render target will be gamma corrected. */
		bool HWGamma = false;

		/**
		 * Does the texture need to be vertically flipped because of different screen space coordinate systems.	(Determines
		 * is origin top left or bottom left. Engine default is top left.)
		 */
		bool RequiresTextureFlipping = false;

		/** True if the target is a window, false if an offscreen target. */
		bool IsWindow = false;

		/** Controls how many samples are used for multisampling. (0 or 1 if multisampling is not used). */
		UINT32 MultisampleCount = 0;
	};

    /**
	 * Provides access to internal render target implementation
	 */
	class TE_CORE_EXPORT RenderTarget
	{
	public:
		/** Frame buffer type when double-buffering is used. */
		enum FrameBuffer
		{
			FB_FRONT,
			FB_BACK,
			FB_AUTO
		};

        RenderTarget() = default;
		virtual ~RenderTarget() = default;

        /** Event that gets triggered whenever the render target is resized. */
		mutable Event<void()> OnResized;

        /** Swaps the frame buffers to display the next frame. */
		virtual void SwapBuffers() {}

        /** Queries the render target for a custom attribute. This may be anything and is implementation specific. */
		virtual void GetCustomAttribute(const String& name, void* pData) const;

		/**	Returns properties that describe the render target. */
		virtual const RenderTargetProperties& GetProperties() const = 0;
    };
}
