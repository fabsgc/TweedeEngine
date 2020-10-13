#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"
#include "Utility/TeEvent.h"
#include "Platform/TePlatform.h"

namespace te
{
    class TE_CORE_EXPORT GuiAPI : public Module<GuiAPI>
    {
    public:
        TE_MODULE_STATIC_HEADER_MEMBER(GuiAPI)

        GuiAPI() = default;
        ~GuiAPI() = default;

        /**
          * Initialize Gui context using parameters in data. Data must be a void pointer because GuiAPI is an interface which can be used 
          * in a DirectX or OpenGL context on both Win32 and Linux
         */
        virtual void Initialize(void* data);
         
        /** Destroy */
        virtual void Destroy();

        /** Update */
        virtual void Update();

        /** Begin a new Gui frame */
        virtual void Begin() = 0;

        /** End current Gui frame */
        virtual void End() = 0;
    public:
        /** Called from the message loop to notify user has entered a character. */
        virtual void CharInput(UINT32 character) = 0;

        /** Called from the message loop to notify user has moved the cursor. */
        virtual void CursorMoved(const Vector2I& cursorPos, const OSPointerButtonStates& btnStates) = 0;

        /** Called from the message loop to notify user has pressed a mouse button. */
        virtual void CursorPressed(const Vector2I& cursorPos, OSMouseButton button, const OSPointerButtonStates& btnStates) = 0;

        /** Called from the message loop to notify user has released a mouse button. */
        virtual void CursorReleased(const Vector2I& cursorPos, OSMouseButton button, const OSPointerButtonStates& btnStates) = 0;

        /** Called from the message loop to notify user has double-clicked a mouse button. */
        virtual void CursorDoubleClick(const Vector2I& cursorPos, const OSPointerButtonStates& btnStates) = 0;

        /** Called from the message loop to notify user has scrolled the mouse wheel. */
        virtual void MouseWheelScrolled(float scrollPos) = 0;

        /** Called from the message loop to notify user has entered a character. */
        virtual void KeyUp(UINT32 character) = 0;

        /** Called from the message loop to notify user has entered a character. */
        virtual void KeyDown(UINT32 character) = 0;

    protected:
        /** Triggered whenever a button is first pressed. */
        Event<void(const ButtonEvent&)> OnButtonDown;

        /**	Triggered whenever a button is first released. */
        Event<void(const ButtonEvent&)> OnButtonUp;

        /**	Triggered whenever user inputs a text character. */
        Event<void(const TextInputEvent&)> OnCharInput;

        /**	Triggers when some pointing device (mouse cursor, touch) moves. */
        Event<void(const PointerEvent&)> OnPointerMoved;

        /**	Triggers when some pointing device (mouse cursor, touch) has a relative move. */
        Event<void(const Vector2I&)> OnPointerRelativeMoved;

        /**	Triggers when some pointing device (mouse cursor, touch) button is pressed. */
        Event<void(const PointerEvent&)> OnPointerPressed;

        /**	Triggers when some pointing device (mouse cursor, touch) button is released. */
        Event<void(const PointerEvent&)> OnPointerReleased;

        /**	Triggers when some pointing device (mouse cursor, touch) button is double clicked. */
        Event<void(const PointerEvent&)> OnPointerDoubleClick;

    protected:
        // OS input events
        HEvent _charInputConn;
        HEvent _cursorMovedConn;
        HEvent _cursorPressedConn;
        HEvent _cursorReleasedConn;
        HEvent _cursorDoubleClickConn;
        HEvent _inputCommandConn;
        HEvent _mouseWheelScrolledConn;
    };
}
