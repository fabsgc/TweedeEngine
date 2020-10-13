#include "TeGLImGuiAPI.h"

namespace te
{
    GLImGuiAPI::GLImGuiAPI()
    { }

    GLImGuiAPI::~GLImGuiAPI()
    { }

    void GLImGuiAPI::Initialize(void* data)
    { 
        GuiAPI::Initialize(data);
    }

    void GLImGuiAPI::Destroy()
    {
        GuiAPI::Destroy();
    }

    void GLImGuiAPI::Update()
    {
        GuiAPI::Update();
    }

    void GLImGuiAPI::Begin()
    { }

    void GLImGuiAPI::End()
    { }

    /** Called from the message loop to notify user has entered a character. */
    void GLImGuiAPI::CharInput(UINT32 character)
    { }

    /** Called from the message loop to notify user has moved the cursor. */
    void GLImGuiAPI::CursorMoved(const Vector2I& cursorPos, const OSPointerButtonStates& btnStates)
    { }

    /** Called from the message loop to notify user has pressed a mouse button. */
    void GLImGuiAPI::CursorPressed(const Vector2I& cursorPos, OSMouseButton button, const OSPointerButtonStates& btnStates)
    { }

    /** Called from the message loop to notify user has released a mouse button. */
    void GLImGuiAPI::CursorReleased(const Vector2I& cursorPos, OSMouseButton button, const OSPointerButtonStates& btnStates)
    { }

    /** Called from the message loop to notify user has double-clicked a mouse button. */
    void GLImGuiAPI::CursorDoubleClick(const Vector2I& cursorPos, const OSPointerButtonStates& btnStates)
    { }

    /** Called from the message loop to notify user has scrolled the mouse wheel. */
    void GLImGuiAPI::MouseWheelScrolled(float scrollPos)
    { }
}
