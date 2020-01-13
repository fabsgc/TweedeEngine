#pragma once

#include "TeCorePrerequisites.h"
#include "RenderAPI/TeRenderTarget.h"
#include "RenderAPI/TeVideoMode.h"
#include "Utility/TeEvent.h"

namespace te
{
    /** Types of events that a RenderWindow can be notified of. */
    enum class WindowEventType
    {
        /** Triggered when window size changes. */
        Resized,
        /** Triggered when window position changes. */
        Moved,
        /** Triggered when window receives input focus. */
        FocusReceived,
        /** Triggered when window loses input focus. */
        FocusLost,
        /** Triggered when the window is minimized (iconified). */
        Minimized,
        /** Triggered when the window is expanded to cover the current screen. */
        Maximized,
        /** Triggered when the window leaves minimized or maximized state. */
        Restored,
        /** Triggered when the mouse pointer leaves the window area. */
        MouseLeft,
        /** Triggered when the user wants to close the window. */
        CloseRequested,
    };

    /** Structure that is used for initializing a render window. */
    struct TE_CORE_EXPORT RENDER_WINDOW_DESC
    {
        RENDER_WINDOW_DESC()
            : Fullscreen(false), Vsync(false), Hidden(false), DepthBuffer(true)
            , MultisampleCount(0), MultisampleHint(""), Gamma(false), Left(-1), Top(-1), Title("Application")
            , ShowTitleBar(true), ShowBorder(true), AllowResize(true), ToolWindow(false), Modal(false)
            , HideUntilSwap(false)
        { }

        VideoMode Mode; /**< Output monitor, frame buffer resize and refresh rate. */
        bool Fullscreen; /**< Should the window be opened in fullscreen mode. */
        bool Vsync; /**< Should the window wait for vertical sync before swapping buffers. */
        bool Hidden; /**< Should the window be hidden initially. */
        bool DepthBuffer; /**< Should the window be created with a depth/stencil buffer. */
        UINT32 MultisampleCount; /**< If higher than 1, texture containing multiple samples per pixel is created. */
        String MultisampleHint; /**< Hint about what kind of multisampling to use. Render system specific. */
        bool Gamma; /**< Should the written color pixels be gamma corrected before write. */
        INT32 Left; /**< Window origin on X axis in pixels. -1 == screen center. Relative to monitor provided in videoMode. */
        INT32 Top; /**< Window origin on Y axis in pixels. -1 == screen center. Relative to monitor provided in videoMode. */
        String Title; /**< Title of the window. */
        bool ShowTitleBar; /**< Determines if the title-bar should be shown or not. */
        bool ShowBorder; /**< Determines if the window border should be shown or not. */
        bool AllowResize; /**< Determines if the user can resize the window by dragging on the window edges. */
        bool ToolWindow; /**< Tool windows have no task bar entry and always remain on top of their parent window. */
        bool Modal; /**< When a modal window is open all other windows will be locked until modal window is closed. */
        bool HideUntilSwap; /**< Window will be created as hidden and only be shown when the first framebuffer swap happens. */
    };

    /**	Contains various properties that describe a render window. */
    class TE_CORE_EXPORT RenderWindowProperties : public RenderTargetProperties
    {
    public:
        RenderWindowProperties(const RENDER_WINDOW_DESC& desc);
        virtual ~RenderWindowProperties() = default;

        /**	True if window is running in fullscreen mode. */
        bool IsFullScreen = false;

        /**	Horizontal origin of the window in pixels. */
        INT32 Left = 0;

        /**	Vertical origin of the window in pixels. */
        INT32 Top = 0;

        /**	Indicates whether the window currently has keyboard focus. */
        bool HasFocus = true;

        /**	True if the window is hidden. */
        bool IsHidden = false;

        /**	True if the window is maximized. */
        bool IsMaximized = false;
    };

    class TE_CORE_EXPORT RenderWindow : public RenderTarget
    {
    public:
        RenderWindow(const RENDER_WINDOW_DESC& desc);
        ~RenderWindow();

        void TriggerCallback();

        virtual void Initialize() {};

        /** Queries the render target for a custom attribute. This may be anything and is implementation specific. */
        virtual void GetCustomAttribute(const String& name, void* pData) const {}

        /**	Returns properties that describe the render window. */
        virtual const RenderWindowProperties& GetProperties() const override { return _properties; }

        /**	Returns properties that describe the render window. */
        virtual const RENDER_WINDOW_DESC& GetDesc() const { return _desc; }

        /**	Converts screen position into window local position. */
        virtual Vector2I ScreenToWindowPos(const Vector2I& screenPos) const = 0;

        /**	Converts window local position to screen position. */
        virtual Vector2I WindowToScreenPos(const Vector2I& windowPos) const = 0;

        /**
         * Resize the window to specified width and height in pixels.
         *
         * @param[in]	width		Width of the window in pixels.
         * @param[in]	height		Height of the window in pixels.
         */
        virtual void Resize(UINT32 width, UINT32 height) {}

        /**	Hide or show the window. */
        virtual void SetHidden(bool hidden);

        /**
         * Makes the render target active or inactive. (for example in the case of a window, it will hide or restore the
         * window).
         */
        virtual void SetActive(bool state) {}

        /**
         * Move the window to specified screen coordinates.
         *
         * @param[in]	left		Position of the left border of the window on the screen.
         * @param[in]	top			Position of the top border of the window on the screen.
         */
        virtual void Move(INT32 left, INT32 top) {}

        /**
         * Hides the window.
         */
        virtual void Hide();

        /**
         * Shows a previously hidden window.
         */
        virtual void Show();

        /**	Minimizes the window to the taskbar. */
        virtual void Minimize() {}

        /**	Maximizes the window over the entire current screen. */
        virtual void Maximize() {}

        /**	Restores the window to original position and size if it is minimized or maximized. */
        virtual void Restore() {}

        /**
         * Switches the window to fullscreen mode. Child windows cannot go into fullscreen mode.
         *
         * @param[in]	width		Width of the window frame buffer in pixels.
         * @param[in]	height		Height of the window frame buffer in pixels.
         * @param[in]	refreshRate	Refresh rate of the window in Hertz.
         * @param[in]	monitorIdx	Index of the monitor to go fullscreen on.
         *
         * @note	If the exact provided mode isn't available, closest one is used instead.
         */
        virtual void SetFullscreen(UINT32 width, UINT32 height, float refreshRate = 60.0f, UINT32 monitorIdx = 0) {}

        /**
         * Switches the window to fullscreen mode. Child windows cannot go into fullscreen mode.
         *
         * @param[in]	videoMode	Mode retrieved from VideoModeInfo in RenderAPI.
         */
        virtual void SetFullscreen(const VideoMode& videoMode) {}

        /**
         * Switches the window to windowed mode.
         *
         * @param[in]	width	Window width in pixels.
         * @param[in]	height	Window height in pixels.
         */
        virtual void SetWindowed(UINT32 width, UINT32 height) {}

        /** Allows to change titlebar of the window. */
        virtual void SetTitle(const String& title) {}

        /** Closes and destroys the window. */
        void Destroy();

    public:
        /** Notifies the window that a specific event occurred. Usually called by the platform specific main event loop. */
        void NotifyWindowEvent(WindowEventType type);

        /** Method that triggers whenever the window changes size or position. */
        virtual void WindowMovedOrResized() { }

        /** Called by the core thread when window is destroyed. */
        void NotifyWindowDestroyed();

        /**	Called by the core thread when window receives focus. */
        void NotifyFocusReceived();

        /**	Called by the core thread when window loses focus. */
        void NotifyFocusLost();

        /**	Called by the core thread when window is moved or resized. */
        void NotifyMovedOrResized();

        /**	Called by the core thread when mouse leaves a window. */
        void NotifyMouseLeft();

        /** Called by the core thread when the user requests for the window to close. */
        void NotifyCloseRequested();

    public:
        /** Event that is triggered when a window gains focus. */
        Event<void(RenderWindow&)> OnFocusGained;

        /**	Event that is triggered when a window loses focus. */
        Event<void(RenderWindow&)> OnFocusLost;

        /**	Event that is triggered when mouse leaves a window. */
        Event<void(RenderWindow&)> OnMouseLeftWindow;

        /** Triggers when the OS requests that the window is closed (e.g. user clicks on the X button in the title bar). */
        Event<void()> OnCloseRequested;

        /** Event that gets triggered whenever the render target is resized. */
        Event<void()> OnResized;

    protected:
        RENDER_WINDOW_DESC _desc;
        mutable Mutex _windowMutex;
        RenderWindowProperties _properties;

        bool _moveOrResized;
        bool _mouseLeft;
        bool _closeRequested;
        bool _focusReceived;
        bool _focusLost;
    };
}