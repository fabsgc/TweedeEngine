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

        void Initialize() override;
        void InitializeGui() override;
        void GetCustomAttribute(const String& name, void* pData) const override;
        void WindowMovedOrResized() override;

        /** @copydoc RenderWindow::Move */
        void Move(INT32 left, INT32 top) override;

        /** @copydoc RenderWindow::Resize */
        void Resize(UINT32 width, UINT32 height) override;

        /** @copydoc RenderWindow::SetHidden */
        void SetHidden(bool hidden) override;

        /** @copydoc RenderWindow::SetActive */
        void SetActive(bool state) override;

        /** @copydoc RenderWindow::Minimize */
        void Minimize() override;

        /** @copydoc RenderWindow::Maximize */
        void Maximize() override;

        /** @copydoc RenderWindow::Restore */
        void Restore() override;

        /** @copydoc RenderWindow::SetVSync */
        void setVSync(bool enabled) override;

        /** @copydoc RenderWindow::SetFullscreen(UINT32, UINT32, float, UINT32) */
        void SetFullscreen(UINT32 width, UINT32 height, float refreshRate = 60.0f, UINT32 monitorIdx = 0) override;

        /** @copydoc RenderWindow::SetFullscreen(const VideoMode&) */
        void SetFullscreen(const VideoMode& videoMode) override;

        /** @copydoc RenderWindow::SetWindowed */
        void SetWindowed(UINT32 width, UINT32 height) override;

        /** @copydoc RenderWindow::ScreenToWindowPos */
        Vector2I ScreenToWindowPos(const Vector2I& screenPos) const override;

        /** @copydoc RenderWindow::WindowToScreenPos */
        Vector2I WindowToScreenPos(const Vector2I& windowPos) const override;

        /** @copydoc RenderWindow::SetTitle */
        void SetTitle(const String& title) override;

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
