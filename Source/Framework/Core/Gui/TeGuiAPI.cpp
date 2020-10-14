#include "TeGuiAPI.h"
#include "Input/TeInput.h"

using namespace std::placeholders;

namespace te
{
    TE_MODULE_STATIC_MEMBER(GuiAPI)

    void GuiAPI::Initialize(void* data)
    {
        _charInputConn = Platform::OnCharInput.Connect(std::bind(&GuiAPI::CharInput, this, _1));
        _cursorMovedConn = Platform::OnCursorMoved.Connect(std::bind(&GuiAPI::CursorMoved, this, _1, _2));
        _cursorPressedConn = Platform::OnCursorButtonPressed.Connect(std::bind(&GuiAPI::CursorPressed, this, _1, _2, _3));
        _cursorReleasedConn = Platform::OnCursorButtonReleased.Connect(std::bind(&GuiAPI::CursorReleased, this, _1, _2, _3));
        _mouseWheelScrolledConn = Platform::OnMouseWheelScrolled.Connect(std::bind(&GuiAPI::MouseWheelScrolled, this, _1));
        _keyUpConn = Platform::OnKeyUp.Connect(std::bind(&GuiAPI::KeyUp, this, _1));
        _keyDownConn = Platform::OnKeyDown.Connect(std::bind(&GuiAPI::KeyDown, this, _1));
    }

    void GuiAPI::Destroy()
    {
        _charInputConn.Disconnect();
        _cursorMovedConn.Disconnect();
        _cursorPressedConn.Disconnect();
        _cursorReleasedConn.Disconnect();
        _mouseWheelScrolledConn.Disconnect();
        _keyUpConn.Disconnect();
        _keyDownConn.Disconnect();
    }

    void GuiAPI::Update()
    { }
}
