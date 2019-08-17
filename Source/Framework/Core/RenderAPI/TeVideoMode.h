#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    /**
	 * Video mode contains information about how a render window presents its information to an output device like a 
	 * monitor.
	 */
	class TE_CORE_EXPORT VideoMode
	{
	public:
		VideoMode() {}

		/**
		 * Creates a new video mode.
		 *
		 * @param[in]	width		Width of the frame buffer in pixels.
		 * @param[in]	height		Height of the frame buffer in pixels.
		 * @param[in]	refreshRate	How often should the output device refresh the output image in hertz.
		 *							represents the primary device while order of others is undefined.
		 */
		VideoMode(UINT32 width, UINT32 height, float refreshRate = 60.0f);
		virtual ~VideoMode();

		bool operator== (const VideoMode& other) const;

		/**	Width of the front/back buffer in pixels. */
		UINT32 GetWidth() const { return _width; }

		/**	Height of the front/back buffer in pixels. */
		UINT32 GetHeight() const { return _height; }

		/**	Returns a refresh rate in hertz. */
		virtual float GetRefreshRate() const { return _refreshRate; }

	protected:
		UINT32 _width = 1280;
		UINT32 _height = 720;
		float _refreshRate = 60.0f;
	};
}