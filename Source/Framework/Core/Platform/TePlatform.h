#pragma once

#include "TeCorePrerequisites.h"
#include "Input/TeInputData.h"

namespace te
{
    /** Types of mouse buttons provided by the OS. */
    enum class OSMouseButton
    {
        Left, Middle, Right, Count
    };

    /** Describes pointer (mouse, touch) states as reported by the OS. */
    struct TE_CORE_EXPORT OSPointerButtonStates
    {
        OSPointerButtonStates()
        {
            mouseButtons[0] = false;
            mouseButtons[1] = false;
            mouseButtons[2] = false;

            shift = false;
            ctrl = false;
        }

        bool mouseButtons[(UINT32)OSMouseButton::Count];
        bool shift, ctrl;
    };

    class TE_CORE_EXPORT Platform
    {
    public:
        struct Pimpl;

        Platform() = default;
        virtual ~Platform();

        /**
         * Retrieves the cursor position in screen coordinates.
         */
        static Vector2I GetCursorPosition();

        /**
         * Moves the cursor to the specified screen position.
         */
        static void SetCursorPosition(const Vector2I& screenPos);

        /**
         * Capture mouse to this window so that we get mouse input even if the mouse leaves the window area.
         */
        static void CaptureMouse(const RenderWindow& window);

        /**
         * Releases the mouse capture set by captureMouse().
         */
        static void ReleaseMouseCapture();

        /**
         * Checks if provided over screen position is over the specified window.
         */
        static bool IsPointOverWindow(const RenderWindow& window, const Vector2I& screenPos);

        /**
         * Limit cursor movement to the specified window.
         */
        static void ClipCursorToWindow(const RenderWindow& window);

        /**
         * Clip cursor to specific area on the screen.
         */

        static void ClipCursorToRect(const Rect2I& screenRect);

        /**
         * Disables cursor clipping.
         */
        static void ClipCursorDisable();

        /**
         * Hides the cursor.
         */
        static void HideCursor();

        /**
         * Shows the cursor.
         */
        static void ShowCursor();

        /**
         * Query if the cursor is hidden.
         */
        static bool IsCursorHidden();

        /** Called during application start up from the sim thread. Must be called before any other operations are done. */
		static void StartUp();

        /** 
         * Called once per frame
         */
        static void Update();

        /**
         * Message pump. Processes OS messages and returns when it's free.
         */
        static void MessagePump();

    public:
        /**
         * Triggered whenever the pointer moves.
         */
        static Event<void(const Vector2I&, const OSPointerButtonStates&)> OnCursorMoved;

        /**
         * Triggered whenever a pointer button is pressed.
         */
        static Event<void(const Vector2I&, OSMouseButton button, const OSPointerButtonStates&)> OnCursorButtonPressed;

        /**
         * Triggered whenever pointer button is released.
         */
        static Event<void(const Vector2I&, OSMouseButton button, const OSPointerButtonStates&)> OnCursorButtonReleased;

        /**
         * Triggered whenever a pointer button is double clicked.
         */
        static Event<void(const Vector2I&, const OSPointerButtonStates&)> OnCursorDoubleClick;

        /**
         * Triggered whenever the mouse wheel is scolled.
         */
        static Event<void(float)> OnMouseWheelScrolled;

        /**
         * Triggered whenever a character is entered.
         */
        static Event<void(UINT32)> OnCharInput;

        /**
         * Triggered whenever mouse capture state for the window is changed (it receives or loses it).
         */
        static Event<void()> OnMouseCaptureChanged;

    protected:
        static Pimpl* _data;
    };
}