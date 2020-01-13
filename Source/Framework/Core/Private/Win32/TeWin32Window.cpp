#include "Private/Win32/TeWin32Window.h"

namespace te
{
    struct Win32Window::Pimpl
    {
        HWND HWnd = nullptr;
        INT32 Left = 0;
        INT32 Top = 0;
        UINT32 Width = 0;
        UINT32 Height = 0;
        bool IsHidden = false;
        DWORD Style = 0;
        DWORD StyleEx = 0;
    };

    Win32Window::Win32Window(const WINDOW_DESC& desc)
    {
        _data = te_new<Pimpl>();
        _data->IsHidden = desc.Hidden;

        HMONITOR hMonitor = desc.Monitor;

        {
            _data->Style = WS_CLIPCHILDREN;

            INT32 left = desc.Left;
            INT32 top = desc.Top;

            if (hMonitor == nullptr)
            {
                POINT windowAnchorPoint;

                // Fill in anchor point.
                windowAnchorPoint.x = left;
                windowAnchorPoint.y = top;

                // Get the nearest monitor to this window.
                hMonitor = MonitorFromPoint(windowAnchorPoint, MONITOR_DEFAULTTOPRIMARY);
            }

            // Get the target monitor info
            MONITORINFO monitorInfo;
            memset(&monitorInfo, 0, sizeof(MONITORINFO));
            monitorInfo.cbSize = sizeof(MONITORINFO);
            GetMonitorInfo(hMonitor, &monitorInfo);

            UINT32 width = desc.Width;
            UINT32 height = desc.Height;

            // No specified top left -> Center the window in the middle of the monitor
            if (left == -1 || top == -1)
            {
                int screenw = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
                int screenh = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

                // clamp window dimensions to screen size
                int outerw = (int(width) < screenw) ? int(width) : screenw;
                int outerh = (int(height) < screenh) ? int(height) : screenh;

                if (left == -1)
                    left = monitorInfo.rcWork.left + (screenw - outerw) / 2;
                else if (hMonitor != nullptr)
                    left += monitorInfo.rcWork.left;

                if (top == -1)
                    top = monitorInfo.rcWork.top + (screenh - outerh) / 2;
                else if (hMonitor != nullptr)
                    top += monitorInfo.rcWork.top;
            }
            else if (hMonitor != nullptr)
            {
                left += monitorInfo.rcWork.left;
                top += monitorInfo.rcWork.top;
            }

            if (!desc.Fullscreen)
            {
                if (desc.ShowTitleBar)
                {
                    if (desc.ShowBorder || desc.AllowResize)
                        _data->Style |= WS_OVERLAPPEDWINDOW;
                    else
                        _data->Style |= WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
                }
                else
                {
                    if (desc.ShowBorder || desc.AllowResize)
                        _data->Style |= WS_POPUP | WS_BORDER;
                    else
                        _data->Style |= WS_POPUP;
                }

                if (!desc.OuterDimensions)
                {
                    // Calculate window dimensions required to get the requested client area
                    RECT rect;
                    SetRect(&rect, 0, 0, width, height);
                    AdjustWindowRect(&rect, _data->Style, false);
                    width = rect.right - rect.left;
                    height = rect.bottom - rect.top;

                    // Clamp width and height to the desktop dimensions
                    int screenw = GetSystemMetrics(SM_CXSCREEN);
                    int screenh = GetSystemMetrics(SM_CYSCREEN);

                    if ((int)width > screenw)
                        width = screenw;

                    if ((int)height > screenh)
                        height = screenh;

                    if (left < 0)
                        left = (screenw - width) / 2;

                    if (top < 0)
                        top = (screenh - height) / 2;
                }
            }
            else
            {
                _data->Style |= WS_POPUP;
                top = 0;
                left = 0;
            }

            UINT classStyle = 0;
            if (desc.EnableDoubleClick)
                classStyle |= CS_DBLCLKS;

            // Register the window class
            WNDCLASS wc = { classStyle, desc.WndProc, 0, 0, desc.Module,
                LoadIcon(nullptr, IDI_APPLICATION), LoadCursor(nullptr, IDC_ARROW),
                (HBRUSH)GetStockObject(BLACK_BRUSH), 0, "Win32Wnd" };

            RegisterClass(&wc);

            _data->HWnd = CreateWindowEx(_data->StyleEx, "Win32Wnd", desc.Title.c_str(), _data->Style,
                left, top, width, height, nullptr, nullptr, desc.Module, desc.CreationParams);
        }

        RECT rect;
        GetWindowRect(_data->HWnd, &rect);
        _data->Top = rect.top;
        _data->Left = rect.left;

        GetClientRect(_data->HWnd, &rect);
        _data->Width = rect.right;
        _data->Height = rect.bottom;

        SetFocus(_data->HWnd);

        if (desc.Hidden)
        {
            SetHidden(true);
        } 
    }

    Win32Window::~Win32Window()
    {
    }

    void Win32Window::Destroy()
    {
        if (_data->HWnd)
        {
            DestroyWindow(_data->HWnd);
        }

        te_delete(_data);
        _data = nullptr;
    }

    INT32 Win32Window::GetLeft() const
    {
        return _data->Left;
    }

    INT32 Win32Window::GetTop() const
    {
        return _data->Top;
    }

    UINT32 Win32Window::GetWidth() const
    {
        return _data->Width;
    }

    UINT32 Win32Window::GetHeight() const
    {
        return _data->Height;
    }

    HWND Win32Window::GetHWnd() const
    {
        return _data->HWnd;
    }

    DWORD Win32Window::GetStyle() const
    {
        return _data->Style;
    }

    DWORD Win32Window::GetStyleEx() const
    {
        return _data->StyleEx;
    }

    void Win32Window::SetHidden(bool hidden)
    {
        if (hidden)
        {
            ShowWindow(_data->HWnd, SW_HIDE);
        }  
        else
        {
            ShowWindow(_data->HWnd, SW_SHOW);
        } 

        _data->IsHidden = hidden;
    }

    void Win32Window::SetActive(bool state)
    {
        if (_data->HWnd)
        {
            if (state)
            {
                ShowWindow(_data->HWnd, SW_RESTORE);
            }
            else
            {
                ShowWindow(_data->HWnd, SW_SHOWMINNOACTIVE);
            }
        }
    }

    void Win32Window::Minimize()
    {
        if (_data->HWnd)
        {
            ShowWindow(_data->HWnd, SW_MINIMIZE);
        }

        if (_data->IsHidden)
        {
            ShowWindow(_data->HWnd, SW_HIDE);
        }
    }

    void Win32Window::Maximize()
    {
        if (_data->HWnd)
        {
            ShowWindow(_data->HWnd, SW_MAXIMIZE);
        }

        if (_data->IsHidden)
        {
            ShowWindow(_data->HWnd, SW_HIDE);
        }
    }

    void Win32Window::Restore()
    {
        if (_data->HWnd)
        {
            ShowWindow(_data->HWnd, SW_RESTORE);
        }

        if (_data->IsHidden)
        {
            ShowWindow(_data->HWnd, SW_HIDE);
        }
    }

    void Win32Window::Resize(UINT32 width, UINT32 height)
    {
        if (_data->HWnd)
        {
            RECT rc = { 0, 0, (LONG)width, (LONG)height };
            AdjustWindowRect(&rc, GetWindowLong(_data->HWnd, GWL_STYLE), false);
            width = rc.right - rc.left;
            height = rc.bottom - rc.top;

            _data->Width = width;
            _data->Height = height;

            SetWindowPos(_data->HWnd, HWND_TOP, _data->Left, _data->Top, width, height, SWP_NOMOVE);
        }
    }

    void Win32Window::Move(INT32 left, INT32 top)
    {
        if (_data->HWnd)
        {
            _data->Top = top;
            _data->Left = left;

            SetWindowPos(_data->HWnd, HWND_TOP, left, top, _data->Width, _data->Height, SWP_NOSIZE);
        }
    }

    Vector2I Win32Window::ScreenToWindowPos(const Vector2I& screenPos) const
    {
        POINT pos;
        pos.x = screenPos.x;
        pos.y = screenPos.y;

        ScreenToClient(_data->HWnd, &pos);
        return Vector2I(pos.x, pos.y);
    }

    Vector2I Win32Window::windowToScreenPos(const Vector2I& windowPos) const
    {
        POINT pos;
        pos.x = windowPos.x;
        pos.y = windowPos.y;

        ClientToScreen(_data->HWnd, &pos);
        return Vector2I(pos.x, pos.y);
    }

    void Win32Window::WindowMovedOrResized()
    {
        if (!_data->HWnd || IsIconic(_data->HWnd))
        {
            return;
        }

        RECT rc;
        GetWindowRect(_data->HWnd, &rc);
        _data->Top = rc.top;
        _data->Left = rc.left;

        GetClientRect(_data->HWnd, &rc);
        _data->Width = rc.right - rc.left;
        _data->Height = rc.bottom - rc.top;
    }
}