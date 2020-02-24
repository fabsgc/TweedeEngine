#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"
#include "Math/TeVector2I.h"
#include "windows.h"

namespace te
{
    /** Descriptor used for creating a platform specific native window. */
    struct TE_UTILITY_EXPORT WINDOW_DESC
    {
        WINDOW_DESC() = default;

        HINSTANCE Module = nullptr; /**< Instance to the local module. */
        HMONITOR Monitor = nullptr; /**< Handle ot the monitor onto which to display the window. */
        void* CreationParams = nullptr; /**< Parameter that will be passed through the WM_CREATE message. */
        UINT32 Width = 0; /**< Width of the window in pixels. */
        UINT32 Height = 0; /**< Height of the window in pixels. */
        bool Fullscreen = false; /**< Should the window be opened in fullscreen mode. */
        bool Hidden = false; /**< Should the window be hidden initially. */
        INT32 Left = -1; /**< Window origin on X axis in pixels. -1 == screen center. Relative to provided monitor. */
        INT32 Top = -1; /**< Window origin on Y axis in pixels. -1 == screen center. Relative to provided monitor. */
        String Title; /**< Title of the window. */
        bool ShowTitleBar = true; /**< Determines if the title-bar should be shown or not. */
        bool ShowBorder = true; /**< Determines if the window border should be shown or not. */
        bool AllowResize = true; /**< Determines if the user can resize the window by dragging on the window edges. */
        bool OuterDimensions = false; /**< Do our dimensions include space for things like title-bar and border. */
        bool EnableDoubleClick = true; /**< Does window accept double-clicks. */
        
        WNDPROC WndProc = nullptr; /**< Pointer to a function that handles windows message processing. */
    };

    /** Represents a Windows native window. */
    class TE_UTILITY_EXPORT Win32Window
    {
    public:
        Win32Window(const WINDOW_DESC& desc);
        ~Win32Window();

        void Destroy();
        
        /** Returns position of the left-most border of the window, relative to the screen. */
        INT32 GetLeft() const;

        /** Returns position of the top-most border of the window, relative to the screen. */
        INT32 GetTop() const;

        /** Returns width of the window in pixels. */
        UINT32 GetWidth() const;

        /** Returns height of the window in pixels. */
        UINT32 GetHeight() const;

        /** Returns the native window handle. */
        HWND GetHWnd() const;

        /** Returns the window style flags used for creating it. */
        DWORD GetStyle() const;

        /** Returns the extended window style flags used for creating it. */
        DWORD GetStyleEx() const;

        /** Hide or show the window. */
        void SetHidden(bool hidden);

        /** Restores or minimizes the window. */
        void SetActive(bool state);

        /** Minimizes the window to the taskbar. */
        void Minimize();

        /** Maximizes the window over the entire current screen. */
        void Maximize();

        /** Restores the window to original position and size if it is minimized or maximized. */
        void Restore();

        /** Change the size of the window. */
        void Resize(UINT32 width, UINT32 height);

        /** Reposition the window. */
        void Move(INT32 left, INT32 top);

        /** Converts screen position into window local position. */
        Vector2I ScreenToWindowPos(const Vector2I& screenPos) const;

        /** Converts window local position to screen position. */
        Vector2I windowToScreenPos(const Vector2I& windowPos) const;

        /** Called when window is moved or resized externally. */
        void WindowMovedOrResized();

    private:
        struct Pimpl;
        Pimpl* _data;
    };
}
