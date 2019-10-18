#include "TeWin32RenderWindow.h"
#include "Private/WIn32/TeWin32Platform.h"

namespace te
{
    Win32RenderWindow::Win32RenderWindow(const RENDER_WINDOW_DESC& desc)
        : RenderWindow(desc)
    {
    }

    Win32RenderWindow::~Win32RenderWindow()
    {
        if (_window != nullptr)
        {
            _window->Destroy();
            te_delete(_window);
        }
    }

    void Win32RenderWindow::Update()
    {
    }

    void Win32RenderWindow::Initialize()
    {
        WINDOW_DESC windowDesc;
        windowDesc.ShowTitleBar = _desc.ShowTitleBar;
        windowDesc.ShowBorder = _desc.ShowBorder;
        windowDesc.AllowResize = _desc.AllowResize;
        windowDesc.EnableDoubleClick = true;
        windowDesc.Fullscreen = _desc.Fullscreen;
        windowDesc.Width = _desc.Mode.GetWidth();
        windowDesc.Height = _desc.Mode.GetHeight();
        windowDesc.Hidden = _desc.Hidden || _desc.HideUntilSwap;
        windowDesc.Left = _desc.Left;
        windowDesc.Top = _desc.Top;
        windowDesc.OuterDimensions = false;
        windowDesc.Title = _desc.Title;
        windowDesc.CreationParams = this;
        windowDesc.WndProc = &Win32Platform::_win32WndProc;

#ifdef TE_STATIC_LIB
        windowDesc.Module = GetModuleHandle(NULL);
#else
        windowDesc.Module = GetModuleHandle("TeGLRenderAPI.dll");
#endif

        _window = te_new<Win32Window>(windowDesc);

        _properties.Width = _window->GetWidth();
        _properties.Height = _window->GetHeight();
        _properties.Top = _window->GetTop();
        _properties.Left = _window->GetLeft();

        if (_properties.IsFullScreen)
        {
        }

        //TODO
    }

    void Win32RenderWindow::GetCustomAttribute(const String& name, void* pData) const
    {
        if (name == "WINDOW")
        {
            UINT64 *pWnd = (UINT64*)pData;
            *pWnd = (UINT64)_window->GetHWnd();
            return;
        }

        RenderWindow::GetCustomAttribute(name, pData);
    }

    HWND Win32RenderWindow::GetHWnd() const
    {
        return _window->GetHWnd();
    }

    void Win32RenderWindow::Move(INT32 left, INT32 top)
    {
        if (!_properties.IsFullScreen)
        {
            _window->Move(left, top);

            _properties.Top = _window->GetTop();
            _properties.Left = _window->GetLeft();
        }
    }

    void Win32RenderWindow::Resize(UINT32 width, UINT32 height)
    {
        if (!_properties.IsFullScreen)
        {
            _window->Resize(width, height);

            _properties.Width = _window->GetWidth();
            _properties.Height = _window->GetHeight();
        }
    }

    void Win32RenderWindow::SetHidden(bool hidden)
    {
        _window->SetHidden(hidden);

        RenderWindow::SetHidden(hidden);
    }

    void Win32RenderWindow::SetActive(bool state)
    {
        _window->SetActive(state);
    }

    void Win32RenderWindow::Minimize()
    {
        _window->Minimize();
    }

    void Win32RenderWindow::Maximize()
    {
        _window->Maximize();
    }

    void Win32RenderWindow::Restore()
    {
        _window->Restore();
    }

    void Win32RenderWindow::SetFullscreen(UINT32 width, UINT32 height, float refreshRate, UINT32 monitorIdx)
    {
        //TODO
    }

    void Win32RenderWindow::SetFullscreen(const VideoMode& videoMode)
    {
        //TODO
    }

    void Win32RenderWindow::SetWindowed(UINT32 width, UINT32 height)
    {
        _properties.Width = width;
        _properties.Height = height;
        _properties.IsFullScreen = false;

        NotifyMovedOrResized();
    }

    Vector2I Win32RenderWindow::ScreenToWindowPos(const Vector2I& screenPos) const
    {
        POINT pos;
        pos.x = screenPos.x;
        pos.y = screenPos.y;

        ScreenToClient(GetHWnd(), &pos);
        return Vector2I(pos.x, pos.y);
    }

    Vector2I Win32RenderWindow::WindowToScreenPos(const Vector2I& windowPos) const
    {
        POINT pos;
        pos.x = windowPos.x;
        pos.y = windowPos.y;

        ClientToScreen(GetHWnd(), &pos);
        return Vector2I(pos.x, pos.y);
    }

    void Win32RenderWindow::WindowMovedOrResized()
    {
        if (!_window)
            return;

        _window->WindowMovedOrResized();

        if (_properties.IsFullScreen) // Fullscreen is handled directly by this object
        {
            // TODO
        }
        else
        {
            // TODO
        }
    }
}