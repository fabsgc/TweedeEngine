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
        enum class FocusType
        {
            Keyboard, Mouse
        };

    public:
        TE_MODULE_STATIC_HEADER_MEMBER(GuiAPI)

        GuiAPI();
        ~GuiAPI() = default;

        /**
          * Initialize Gui context using parameters in data. Data must be a void pointer because GuiAPI is an interface which can be used 
          * in a DirectX or OpenGL context on both Win32 and Linux
         */
        virtual void Initialize(void* data);
         
        /** Destroy */
        virtual void Destroy();

        /** Update */
        virtual void Update() { };

        /** Begin a new Gui frame */
        virtual void BeginFrame() = 0;

        /** End current Gui frame */
        virtual void EndFrame() = 0;

        /** Return true if the Gui currently want to listen to event from keyboard or mouse */
        virtual bool HasFocus(FocusType type) = 0;

        /** Before using ImGui somewhere, we want to be sure that gui context is initialized */
        inline bool IsGuiInitialized() { return _guiInitialized; };

    public:
        /** Called from the message loop to notify user has entered a character. */
        virtual void CharInput(UINT32 character) = 0;

        /** Called from the message loop to notify user has moved the cursor. */
        virtual void CursorMoved(const Vector2I& cursorPos, const OSPointerButtonStates& btnStates) = 0;

        /** Called from the message loop to notify user has pressed a mouse button. */
        virtual void CursorPressed(const Vector2I& cursorPos, OSMouseButton button, const OSPointerButtonStates& btnStates) = 0;

        /** Called from the message loop to notify user has released a mouse button. */
        virtual void CursorReleased(const Vector2I& cursorPos, OSMouseButton button, const OSPointerButtonStates& btnStates) = 0;

        /** Called from the message loop to notify user has double click left. */
        virtual void CursorDoubleClick(const Vector2I& cursorPos, const OSPointerButtonStates& btnStates) = 0;

        /** Called from the message loop to notify user has scrolled the mouse wheel. */
        virtual void MouseWheelScrolled(float scrollPos) = 0;

        /** Called from the message loop to notify user has released a key. */
        virtual void KeyUp(UINT32 keyCode) = 0;

        /** Called from the message loop to notify user has released a key. */
        virtual void KeyDown(UINT32 keyCode) = 0;

    protected:
        // OS input events
        HEvent _charInputConn;
        HEvent _cursorMovedConn;
        HEvent _cursorPressedConn;
        HEvent _cursorReleasedConn;
        HEvent _cursorDoubleClickConn;
        HEvent _mouseWheelScrolledConn;
        HEvent _keyDownConn;
        HEvent _keyUpConn;

        bool  _guiInitialized;
        bool  _guiStarted;
        bool  _guiEnded;
    };
}
