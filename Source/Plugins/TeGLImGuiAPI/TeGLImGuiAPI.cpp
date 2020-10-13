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

    void GLImGuiAPI::CharInput(UINT32 character)
    { }

    void GLImGuiAPI::CursorMoved(const Vector2I& cursorPos, const OSPointerButtonStates& btnStates)
    { }

    void GLImGuiAPI::CursorPressed(const Vector2I& cursorPos, OSMouseButton button, const OSPointerButtonStates& btnStates)
    { }

    void GLImGuiAPI::CursorReleased(const Vector2I& cursorPos, OSMouseButton button, const OSPointerButtonStates& btnStates)
    { }

    void GLImGuiAPI::CursorDoubleClick(const Vector2I& cursorPos, const OSPointerButtonStates& btnStates)
    { }

    void GLImGuiAPI::MouseWheelScrolled(float scrollPos)
    { }

    void GLImGuiAPI::ButtonUp(ButtonEvent event)
    { }

    void GLImGuiAPI::ButtonDown(ButtonEvent event)
    { }
}
