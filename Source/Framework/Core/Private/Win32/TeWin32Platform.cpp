#include "Private/Win32/TeWin32Platform.h"
#include "RenderAPI/TeRenderWindow.h"
#include "TeCoreApplication.h"
#include "Math/TeVector2I.h"
#include "Math/TeRect2I.h"
#include "Threading/TeThreading.h"
#include "Utility/TeEvent.h"
#include "Utility/TePlatformUtility.h"
#include <shellapi.h>

namespace te
{
    /** Encapsulate native cursor data so we can avoid including windows.h as it pollutes the global namespace. */
    struct TE_CORE_EXPORT NativeCursorData
    {
        HCURSOR cursor;
    };

    struct Platform::Pimpl
    {
        NativeCursorData Cursor;
        bool IsCursorHidden = false;
        bool IsTrackingMouse = false;

        bool CursorClipping = false;
        HWND ClipWindow = 0;
        RECT ClipRect;

        bool IsActive = false;

        Mutex Sync;
    };

    Event<void(const Vector2I&, const OSPointerButtonStates&)> Platform::OnCursorMoved;
    Event<void(const Vector2I&, OSMouseButton button, const OSPointerButtonStates&)> Platform::OnCursorButtonPressed;
    Event<void(const Vector2I&, OSMouseButton button, const OSPointerButtonStates&)> Platform::OnCursorButtonReleased;
    Event<void(const Vector2I&, const OSPointerButtonStates&)> Platform::OnCursorDoubleClick;
    Event<void(float)> Platform::OnMouseWheelScrolled;
    Event<void(UINT32)> Platform::OnCharInput;

    Event<void()> Platform::OnMouseCaptureChanged;

    Platform::Pimpl* Platform::_data = te_new<Platform::Pimpl>();

    bool IsShiftPressed = false;
    bool IsCtrlPressed = false;

    /** Checks if any of the windows of the current application are active. */
    bool IsAppActive(Platform::Pimpl* data)
    {
        Lock lock(data->Sync);

        return data->IsActive;
    }

    /** Enables or disables cursor clipping depending on the stored data. */
    void ApplyClipping(Platform::Pimpl* data)
    {
        if (data->CursorClipping)
        {
            if (data->ClipWindow)
            {
                // Clip cursor to the window
                RECT clipWindowRect;
                if (GetWindowRect(data->ClipWindow, &clipWindowRect))
                {
                    ClipCursor(&clipWindowRect);
                }
            }
            else
            {
                ClipCursor(&data->ClipRect);
            }
        }
        else
        {
            ClipCursor(nullptr);
        }
    }

    /**	Method triggered whenever a mouse event happens. */
    void GetMouseData(HWND hWnd, WPARAM wParam, LPARAM lParam, bool nonClient, Vector2I& mousePos, OSPointerButtonStates& btnStates)
    {
        POINT clientPoint;

        clientPoint.x = GET_X_LPARAM(lParam);
        clientPoint.y = GET_Y_LPARAM(lParam);

        //We want mouse position relative to screen not to monitor
        //if (!nonClient)
        //    ClientToScreen(hWnd, &clientPoint);

        mousePos.x = clientPoint.x;
        mousePos.y = clientPoint.y;
        
        btnStates.MouseButtons[0] = (wParam & MK_LBUTTON) != 0;
        btnStates.MouseButtons[1] = (wParam & MK_MBUTTON) != 0;
        btnStates.MouseButtons[2] = (wParam & MK_RBUTTON) != 0;
        btnStates.Shift = (wParam & MK_SHIFT) != 0;
        btnStates.Ctrl = (wParam & MK_CONTROL) != 0;
    }

    Platform::~Platform()
    {
    }

    Vector2I Platform::GetCursorPosition()
    {
        Vector2I screenPos;

        POINT cursorPos;
        GetCursorPos(&cursorPos);

        screenPos.x = cursorPos.x;
        screenPos.y = cursorPos.y;

        return screenPos;
    }

    void Platform::SetCursorPosition(const Vector2I& screenPos)
    {
        SetCursorPos(screenPos.x, screenPos.y);
    }

    void Platform::CaptureMouse(const RenderWindow& window)
    {
        SPtr<RenderWindow> win = gCoreApplication().GetWindow();
        UINT64 hwnd;
        win->GetCustomAttribute("WINDOW", &hwnd);

        PostMessage((HWND)hwnd, WM_TE_SETCAPTURE, WPARAM((HWND)hwnd), 0);
    }

    void Platform::ReleaseMouseCapture()
    {
        SPtr<RenderWindow> window = gCoreApplication().GetWindow();
        UINT64 hwnd;
        window->GetCustomAttribute("WINDOW", &hwnd);

        PostMessage((HWND)hwnd, WM_TE_RELEASECAPTURE, WPARAM((HWND)hwnd), 0);
    }

    bool Platform::IsPointOverWindow(const RenderWindow& window, const Vector2I& screenPos)
    {
        POINT point;
        point.x = screenPos.x;
        point.y = screenPos.y;

        UINT64 hwndToCheck;
        window.GetCustomAttribute("WINDOW", &hwndToCheck);

        HWND hwndUnderPos = WindowFromPoint(point);
        return hwndUnderPos == (HWND)hwndToCheck;
    }

    void Platform::ClipCursorToWindow(const RenderWindow& window)
    {
        UINT64 hwnd;
        window.GetCustomAttribute("WINDOW", &hwnd);

        _data->CursorClipping = true;
        _data->ClipWindow = (HWND)hwnd;

        if (IsAppActive(_data))
        {
            ApplyClipping(_data);
        }
    }

    void Platform::ClipCursorToRect(const Rect2I& screenRect)
    {
        _data->CursorClipping = true;
        _data->ClipWindow = 0;

        _data->ClipRect.left = screenRect.x;
        _data->ClipRect.top = screenRect.y;
        _data->ClipRect.right = screenRect.x + screenRect.width;
        _data->ClipRect.bottom = screenRect.y + screenRect.height;

        if (IsAppActive(_data))
        {
            ApplyClipping(_data);
        }
    }

    void Platform::ClipCursorDisable()
    {
        _data->CursorClipping = false;
        _data->ClipWindow = 0;

        if (IsAppActive(_data))
        {
            ApplyClipping(_data);
        }
    }

    void Platform::HideCursor()
    {
        if (_data->IsCursorHidden)
            return;

        _data->IsCursorHidden = true;

        // ShowCursor(FALSE) doesn't work. Presumably because we're in the wrong thread, and using
        // WM_SETCURSOR in message loop to hide the cursor is smarter solution anyway.

        SPtr<RenderWindow> window = gCoreApplication().GetWindow();
        UINT64 hwnd;
        window->GetCustomAttribute("WINDOW", &hwnd);

        PostMessage((HWND)hwnd, WM_SETCURSOR, WPARAM((HWND)hwnd), (LPARAM)MAKELONG(HTCLIENT, WM_MOUSEMOVE));
    }

    void Platform::ShowCursor()
    {
        if (!_data->IsCursorHidden)
            return;

        _data->IsCursorHidden = false;

        // ShowCursor(FALSE) doesn't work. Presumably because we're in the wrong thread, and using
        // WM_SETCURSOR in message loop to hide the cursor is smarter solution anyway.

        SPtr<RenderWindow> primaryWindow = gCoreApplication().GetWindow();
        UINT64 hwnd;
        primaryWindow->GetCustomAttribute("WINDOW", &hwnd);

        PostMessage((HWND)hwnd, WM_SETCURSOR, WPARAM((HWND)hwnd), (LPARAM)MAKELONG(HTCLIENT, WM_MOUSEMOVE));
    }

    bool Platform::IsCursorHidden()
    {
        return _data->IsCursorHidden;
    }

    void Platform::StartUp()
    {
    }

    void Platform::ShutDown()
    {
    }

    void Platform::Update()
    {
        MessagePump();
    }

    void Platform::MessagePump()
    {
        MSG  msg;
        while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    LRESULT CALLBACK Win32Platform::_win32WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if (uMsg == WM_CREATE)
        {	// Store pointer to Win32Window in user data area
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)(((LPCREATESTRUCT)lParam)->lpCreateParams));

            RenderWindow* newWindow = (RenderWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
            if (newWindow != nullptr)
            {
                ShowWindow(hWnd, SW_SHOWNORMAL);
            }
            else
            {
                ShowWindow(hWnd, SW_SHOWNORMAL);
            }

            return 0;
        }

        RenderWindow* win = (RenderWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
        if (!win)
        {
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }

        switch (uMsg)
        {
            case WM_ACTIVATE:
            {
                switch (wParam)
                {
                case WA_ACTIVE:
                case WA_CLICKACTIVE:
                {
                    Lock lock(_data->Sync);
                    _data->IsActive = true;
                }

                ApplyClipping(_data);
                break;
                case WA_INACTIVE:
                {
                    Lock lock(_data->Sync);
                    _data->IsActive = false;
                }

                ClipCursor(nullptr);
                break;
                }

                return 0;
            }
            case WM_SETFOCUS:
            {
                if (!win->GetRenderWindowProperties().HasFocus)
                    win->NotifyWindowEvent(WindowEventType::FocusReceived);

                return 0;
            }
            case WM_KILLFOCUS:
            {
                if (win->GetRenderWindowProperties().HasFocus)
                    win->NotifyWindowEvent(WindowEventType::FocusLost);

                return 0;
            }
            case WM_SYSCHAR:
            {
                if (wParam != VK_SPACE)
                    return 0;
                break;
            }
            case WM_MOVE:
            {
                win->NotifyWindowEvent(WindowEventType::Moved);
                return 0;
            }
            case WM_DISPLAYCHANGE:
            {
                win->NotifyWindowEvent(WindowEventType::Resized);
                break;
            }
            case WM_SIZE:
            {
                win->NotifyWindowEvent(WindowEventType::Resized);

                if (wParam == SIZE_MAXIMIZED)
                    win->NotifyWindowEvent(WindowEventType::Maximized);
                else if (wParam == SIZE_MINIMIZED)
                    win->NotifyWindowEvent(WindowEventType::Minimized);
                else if (wParam == SIZE_RESTORED)
                    win->NotifyWindowEvent(WindowEventType::Restored);

                return 0;
            }
            case WM_SETCURSOR:
            {
                if (IsCursorHidden())
                {
                    SetCursor(nullptr);
                }
                else
                {
                    switch (LOWORD(lParam))
                    {
                    case HTTOPLEFT:
                        SetCursor(LoadCursor(0, IDC_SIZENWSE));
                        return 0;
                    case HTTOP:
                        SetCursor(LoadCursor(0, IDC_SIZENS));
                        return 0;
                    case HTTOPRIGHT:
                        SetCursor(LoadCursor(0, IDC_SIZENESW));
                        return 0;
                    case HTLEFT:
                        SetCursor(LoadCursor(0, IDC_SIZEWE));
                        return 0;
                    case HTRIGHT:
                        SetCursor(LoadCursor(0, IDC_SIZEWE));
                        return 0;
                    case HTBOTTOMLEFT:
                        SetCursor(LoadCursor(0, IDC_SIZENESW));
                        return 0;
                    case HTBOTTOM:
                        SetCursor(LoadCursor(0, IDC_SIZENS));
                        return 0;
                    case HTBOTTOMRIGHT:
                        SetCursor(LoadCursor(0, IDC_SIZENWSE));
                        return 0;
                    default:
                        SetCursor(LoadCursor(0, IDC_ARROW));
                        return 0;
                    }

                    SetCursor(_data->Cursor.cursor);
                }
                return true;
            }

            case WM_GETMINMAXINFO:
            {
                // Prevent the window from going smaller than some minimu size
                ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 250;
                ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 250;

                // Ensure maximizes window has proper size and doesn't cover the entire screen
                const POINT ptZero = { 0, 0 };
                HMONITOR primaryMonitor = MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);

                MONITORINFO monitorInfo;
                monitorInfo.cbSize = sizeof(MONITORINFO);
                GetMonitorInfo(primaryMonitor, &monitorInfo);

                ((MINMAXINFO*)lParam)->ptMaxPosition.x = monitorInfo.rcWork.left - monitorInfo.rcMonitor.left;
                ((MINMAXINFO*)lParam)->ptMaxPosition.y = monitorInfo.rcWork.top - monitorInfo.rcMonitor.top;
                ((MINMAXINFO*)lParam)->ptMaxSize.x = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
                ((MINMAXINFO*)lParam)->ptMaxSize.y = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;
            }
            break;
            case WM_CLOSE:
            {
                win->NotifyWindowEvent(WindowEventType::CloseRequested);

                return 0;
            }
            case WM_NCLBUTTONDBLCLK:
            {
                // Maximize/Restore on double-click
                if (wParam == HTCAPTION)
                {
                    WINDOWPLACEMENT windowPlacement;
                    windowPlacement.length = sizeof(WINDOWPLACEMENT);
                    GetWindowPlacement(hWnd, &windowPlacement);

                    if (windowPlacement.showCmd == SW_MAXIMIZE)
                        ShowWindow(hWnd, SW_RESTORE);
                    else
                        ShowWindow(hWnd, SW_MAXIMIZE);

                    return 0;
                }
                break;
            }
            case WM_MOUSELEAVE:
            {
                // Note: Right now I track only mouse leaving client area. So it's possible for the "mouse left window" callback
                // to trigger, while the mouse is still in the non-client area of the window.
                _data->IsTrackingMouse = false; // TrackMouseEvent ends when this message is received and needs to be re-applied

                Lock lock(_data->Sync);
                win->NotifyWindowEvent(WindowEventType::MouseLeft);

                return 0;
            }
            case WM_LBUTTONUP:
            {
                ReleaseCapture();

                Vector2I intMousePos;
                OSPointerButtonStates btnStates;

                GetMouseData(hWnd, wParam, lParam, false, intMousePos, btnStates);

                if (!OnCursorButtonReleased.Empty())
                    OnCursorButtonReleased(intMousePos, OSMouseButton::Left, btnStates);

                return 0;
            }
            case WM_MBUTTONUP:
            {
                ReleaseCapture();

                Vector2I intMousePos;
                OSPointerButtonStates btnStates;

                GetMouseData(hWnd, wParam, lParam, false, intMousePos, btnStates);

                if (!OnCursorButtonReleased.Empty())
                    OnCursorButtonReleased(intMousePos, OSMouseButton::Middle, btnStates);

                return 0;
            }
            case WM_RBUTTONUP:
            {
                ReleaseCapture();

                Vector2I intMousePos;
                OSPointerButtonStates btnStates;

                GetMouseData(hWnd, wParam, lParam, false, intMousePos, btnStates);

                if (!OnCursorButtonReleased.Empty())
                    OnCursorButtonReleased(intMousePos, OSMouseButton::Right, btnStates);

                return 0;
            }
            case WM_LBUTTONDOWN:
            {
                SetCapture(hWnd);

                Vector2I intMousePos;
                OSPointerButtonStates btnStates;

                GetMouseData(hWnd, wParam, lParam, false, intMousePos, btnStates);

                if (!OnCursorButtonPressed.Empty())
                    OnCursorButtonPressed(intMousePos, OSMouseButton::Left, btnStates);

                return 0;
            }
            case WM_MBUTTONDOWN:
            {
                SetCapture(hWnd);

                Vector2I intMousePos;
                OSPointerButtonStates btnStates;

                GetMouseData(hWnd, wParam, lParam, false, intMousePos, btnStates);

                if (!OnCursorButtonPressed.Empty())
                    OnCursorButtonPressed(intMousePos, OSMouseButton::Middle, btnStates);

                return 0;
            }
            case WM_RBUTTONDOWN:
            {
                SetCapture(hWnd);

                Vector2I intMousePos;
                OSPointerButtonStates btnStates;

                GetMouseData(hWnd, wParam, lParam, false, intMousePos, btnStates);

                if (!OnCursorButtonPressed.Empty())
                    OnCursorButtonPressed(intMousePos, OSMouseButton::Right, btnStates);

                return 0;
            }
            case WM_LBUTTONDBLCLK:
            {
                Vector2I intMousePos;
                OSPointerButtonStates btnStates;

                GetMouseData(hWnd, wParam, lParam, false, intMousePos, btnStates);

                if (!OnCursorDoubleClick.Empty())
                    OnCursorDoubleClick(intMousePos, btnStates);

                return 0;
            }
            case WM_NCMOUSEMOVE:
            case WM_MOUSEMOVE:
            {
                // Set up tracking so we get notified when mouse leaves the window
                if (!_data->IsTrackingMouse)
                {
                    TRACKMOUSEEVENT tme = { sizeof(tme) };
                    tme.dwFlags = TME_LEAVE;

                    tme.hwndTrack = hWnd;
                    TrackMouseEvent(&tme);

                    _data->IsTrackingMouse = true;
                }

                Vector2I intMousePos;
                OSPointerButtonStates btnStates;

                GetMouseData(hWnd, wParam, lParam, uMsg == WM_NCMOUSEMOVE, intMousePos, btnStates);

                if (!OnCursorMoved.Empty())
                    OnCursorMoved(intMousePos, btnStates);

                return 0;
            }
            case WM_MOUSEWHEEL:
            {
                INT16 wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);

                float wheelDeltaFlt = wheelDelta / (float)WHEEL_DELTA;
                if (!OnMouseWheelScrolled.Empty())
                    OnMouseWheelScrolled(wheelDeltaFlt);

                return true;
            }
            case WM_SYSKEYDOWN:
            case WM_KEYDOWN:
            {
                if (wParam == VK_SHIFT)
                {
                    IsShiftPressed = true;
                    break;
                }

                if (wParam == VK_CONTROL)
                {
                    IsCtrlPressed = true;
                    break;
                }

                break;
            }
            case WM_SYSKEYUP:
            case WM_KEYUP:
            {
                if (wParam == VK_SHIFT)
                {
                    IsShiftPressed = false;
                }

                if (wParam == VK_CONTROL)
                {
                    IsCtrlPressed = false;
                }

                return 0;
            }
            case WM_CHAR:
            {
                // Ignore rarely used special command characters, usually triggered by ctrl+key
                // combinations. (We want to keep ctrl+key free for shortcuts instead)
                if (wParam <= 23)
                    break;

                switch (wParam)
                {
                    case VK_ESCAPE:
                        break;
                    default: // displayable character 
                    {
                        UINT32 finalChar = (UINT32)wParam;

                        if (!OnCharInput.Empty())
                            OnCharInput(finalChar);

                        return 0;
                    }
                }

                break;
            }
            case WM_TE_SETCAPTURE:
                SetCapture(hWnd);
                break;
            case WM_TE_RELEASECAPTURE:
                ReleaseCapture();
                break;
            case WM_CAPTURECHANGED:
                if (!OnMouseCaptureChanged.Empty())
                    OnMouseCaptureChanged();
                return 0;
        }

        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}