#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "RenderAPI/TeRenderWindow.h"
#include "Math/TeVector2I.h"

#include <X11/Xutil.h>

namespace te
{
    class LinuxWindow;

    /** Determines which features are supported by a particular framebuffer configuration. */
	struct GLVisualCapabilities
	{
		bool DepthStencil = false;
		UINT32 NumSamples = 1;
		bool Srgb = false;
	};

	/** Contains information about a framebuffer configuration that can be used to initialize a window and GL context. */
	struct GLVisualConfig
	{
		GLVisualCapabilities Caps;
		XVisualInfo VisualInfo;
	};
    
    class LinuxRenderWindow : public RenderWindow
	{
	public:
        LinuxRenderWindow(const RENDER_WINDOW_DESC& desc);
		~LinuxRenderWindow();

        void Update() override;
        void Initialize() override;
        void GetCustomAttribute(const String& name, void* pData) const override;
        void WindowMovedOrResized() override;

        /** @copydoc RenderWindow::move */
        void Move(INT32 left, INT32 top) override;

        /** @copydoc RenderWindow::resize */
        void Resize(UINT32 width, UINT32 height) override;

        /** @copydoc RenderWindow::setHidden */
        void SetHidden(bool hidden) override;

        /** @copydoc RenderWindow::setActive */
        void SetActive(bool state) override;

        /** @copydoc RenderWindow::minimize */
        void Minimize() override;

        /** @copydoc RenderWindow::maximize */
        void Maximize() override;

        /** @copydoc RenderWindow::restore */
        void Restore() override;

        /** @copydoc RenderWindow::setFullscreen(UINT32, UINT32, float, UINT32) */
        void SetFullscreen(UINT32 width, UINT32 height, float refreshRate = 60.0f, UINT32 monitorIdx = 0) override;

        /** @copydoc RenderWindow::setFullscreen(const VideoMode&) */
        void SetFullscreen(const VideoMode& videoMode) override;

        /** @copydoc RenderWindow::setWindowed */
        void SetWindowed(UINT32 width, UINT32 height) override;

        /** @copydoc RenderWindow::screenToWindowPos */
        Vector2I ScreenToWindowPos(const Vector2I& screenPos) const override;

        /** @copydoc RenderWindow::windowToScreenPos */
        Vector2I WindowToScreenPos(const Vector2I& windowPos) const override;

        /**
		 * Selects an appropriate X11 visual info depending on the provided parameters. Visual info should then be used
		 * for creation of an X11 window.
		 *
		 * @param[in] display		X11 display the window will be created on.
		 * @param[in] depthStencil	True if the window requires a depth-stencil buffer.
		 * @param[in] multisample	Number of samples per pixel, if window back buffer requires support for multiple samples.
		 * 							Set to 0 or 1 if multisampling is not required.
		 * @param[in] srgb			If enabled the pixels written to the back-buffer are assumed to be in linear space and
		 * 							will automatically be encoded into gamma space on write.
		 * @return					X11 visual info structure you may use to initialize a window.
		 */
        static GLVisualConfig FindBestVisual(::Display* display, bool depthStencil, UINT32 multisample, bool srgb);

    protected:
        LinuxWindow* _window;
    };
}