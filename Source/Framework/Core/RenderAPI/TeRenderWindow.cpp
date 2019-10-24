#include "RenderAPI/TeRenderWindow.h"
#include "TeCoreApplication.h"

namespace te
{
    RenderWindowProperties::RenderWindowProperties(const RENDER_WINDOW_DESC& desc)
    {
        Width = desc.Mode.GetWidth();
        Height = desc.Mode.GetHeight();
        Vsync = desc.Vsync;
        MultisampleCount = desc.MultisampleCount;
        Left = desc.Left;
        Top = desc.Top;
        IsFullScreen = desc.Fullscreen;
        IsHidden = desc.Hidden;
        RequiresTextureFlipping = false;
    }

    RenderWindow::RenderWindow(const RENDER_WINDOW_DESC& desc)
        : _desc(desc)
        , _properties(desc)
        , _moveOrResized(false)
        , _mouseLeft(false)
        , _closeRequested(false)
        , _focusReceived(false)
        , _focusLost(false)
    {
    }

    RenderWindow::~RenderWindow()
    {
        NotifyWindowDestroyed();
    }

    void RenderWindow::TriggerCallback()
    {
        if(_moveOrResized)
            OnResized();

        if (_mouseLeft)
            OnMouseLeftWindow(*this);

        if (_closeRequested)
        {
            OnCloseRequested();
            gCoreApplication().OnStopRequested();
        }

        if(_focusReceived)
            OnFocusGained(*this);

        if(_focusLost)
            OnFocusLost(*this);

        _moveOrResized = false;
        _mouseLeft = false;
        _closeRequested = false;
        _focusReceived = false;
        _focusLost = false;
    }

    void RenderWindow::SetHidden(bool hidden)
    {
        _properties.IsHidden = hidden;
    }

    void RenderWindow::Hide()
    {
        SetHidden(true);
    }

    void RenderWindow::Show()
    {
        SetHidden(false);
    }

    void RenderWindow::Destroy()
    {
        NotifyCloseRequested();
    }

    void RenderWindow::NotifyWindowEvent(WindowEventType type)
    {
        switch (type)
        {
            case WindowEventType::Resized:
            {
                WindowMovedOrResized();
                NotifyMovedOrResized();
                TE_PRINT("Event resized");
                break;
            }
            case WindowEventType::Moved:
            {
                WindowMovedOrResized();
                TE_PRINT("Event moved");
                break;
            }
            case WindowEventType::FocusReceived:
            {
                NotifyFocusReceived();
                TE_PRINT("Event focus received");
                break;
            }
            case WindowEventType::FocusLost:
            {
                NotifyFocusLost();
                TE_PRINT("Event focus lost");
                break;
            }
            case WindowEventType::Minimized:
            {
                _properties.IsMaximized = false;
                TE_PRINT("Event minimized");
                break;
            }
            case WindowEventType::Maximized:
            {
                _properties.IsMaximized = true;
                TE_PRINT("Event maximized");
                break;
            }
            case WindowEventType::Restored:
            {
                _properties.IsMaximized = false;
                TE_PRINT("Event restored");
                break;
            }
            case WindowEventType::MouseLeft:
            {
                NotifyMouseLeft();
                TE_PRINT("Event mouse left");
                break;
            }
            case WindowEventType::CloseRequested:
            {
                NotifyCloseRequested();
                TE_PRINT("Event close requested");
                break;
            }
        }
    }

    void RenderWindow::NotifyWindowDestroyed()
    {
        // Nothing because only one window in my implementation
    }

    void RenderWindow::NotifyFocusReceived()
    {
        _properties.HasFocus = true;
        _focusReceived = true;
    }

    void RenderWindow::NotifyFocusLost()
    {
        _properties.HasFocus = false;
        _focusLost = true;
    }

    void RenderWindow::NotifyMovedOrResized()
    {
        _moveOrResized = true;
    }

    void RenderWindow::NotifyMouseLeft()
    {
        _mouseLeft = true;
    }

    void RenderWindow::NotifyCloseRequested()
    {
        _closeRequested = true;
    }
}