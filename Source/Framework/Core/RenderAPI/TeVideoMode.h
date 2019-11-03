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

    /** Contains information about a video output device, including a list of all available video modes. */
    class TE_CORE_EXPORT VideoOutputInfo
    {
    public:
        VideoOutputInfo() = default;
        virtual ~VideoOutputInfo();

        VideoOutputInfo(const VideoOutputInfo&) = delete; // Make non-copyable
        VideoOutputInfo& operator=(const VideoOutputInfo&) = delete; // Make non-copyable

        /**	Name of the output device. */
        const String& getName() const { return _name; }

        /**	Number of available video modes for this output. */
        UINT32 getNumVideoModes() const { return (UINT32)_videoModes.size(); }

        /**	Returns video mode at the specified index. */
        const VideoMode& getVideoMode(UINT32 idx) const { return *_videoModes.at(idx); }

        /**	Returns the video mode currently used by the desktop. */
        const VideoMode& getDesktopVideoMode() const { return *_desktopVideoMode; }

    protected:
        String _name;
        Vector<VideoMode*> _videoModes;
        VideoMode* _desktopVideoMode = nullptr;
    };

    /** Contains information about available output devices (for example monitor) and their video modes. */
    class TE_CORE_EXPORT VideoModeInfo
    {
    public:
        VideoModeInfo() = default;
        virtual ~VideoModeInfo();

        VideoModeInfo(const VideoModeInfo&) = delete; // Make non-copyable
        VideoModeInfo& operator=(const VideoModeInfo&) = delete; // Make non-copyable

        /**	Returns the number of available output devices. */
        UINT32 getNumOutputs() const { return (UINT32)_outputs.size(); }

        /**
         * Returns video mode information about a specific output device. 0th index always represents the primary device
         * while order of others is undefined.
         */
        const VideoOutputInfo& getOutputInfo(UINT32 idx) const { return *_outputs[idx]; }

    protected:
        Vector<VideoOutputInfo*> _outputs;
    };
}