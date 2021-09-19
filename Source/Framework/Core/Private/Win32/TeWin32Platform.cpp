#include "Private/Win32/TeWin32Platform.h"
#include "RenderAPI/TeRenderWindow.h"
#include "TeCoreApplication.h"
#include "Math/TeVector2I.h"
#include "Math/TeRect2I.h"
#include "Threading/TeThreading.h"
#include "Utility/TeEvent.h"
#include "Utility/TePlatformUtility.h"
#include "Utility/TeTime.h"
#include "Image/TePixelData.h"
#include "Image/TeColor.h"
#include <shellapi.h>
#include "Utility/TePlatformUtility.h"
#include "Private/Win32/TeWin32PlatformUtility.h"

#ifndef WM_DPICHANGED
#   define WM_DPICHANGED 0x02E0 // From Windows SDK 8.1+ headers
#endif

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
        bool UsingCustomCursor = false;
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
    Event<void(InputCommandType)> Platform::OnInputCommand;
    Event<void(float)> Platform::OnMouseWheelScrolled;
    Event<void(UINT32)> Platform::OnCharInput;
    Event<void(UINT32)> Platform::OnKeyUp;
    Event<void(UINT32)> Platform::OnKeyDown;

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

    /** Method triggered whenever a mouse event happens. */
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
    { }

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

    void Platform::ChangeCursor(PixelData& pixelData, const Vector2I& hotSpot)
    {
        if (_data->UsingCustomCursor)
        {
            SetCursor(0);
            DestroyIcon(_data->Cursor.cursor);
        }

        _data->UsingCustomCursor = true;

        Vector<Color> pixels = pixelData.GetColors();
        UINT32 width = pixelData.GetWidth();
        UINT32 height = pixelData.GetHeight();

        HBITMAP hBitmap = Win32PlatformUtility::CreateBitmap((Color*)pixels.data(), width, height, false);
        HBITMAP hMonoBitmap = CreateBitmap(width, height, 1, 1, nullptr);

        ICONINFO iconinfo = { 0 };
        iconinfo.fIcon = FALSE;
        iconinfo.xHotspot = (DWORD)hotSpot.x;
        iconinfo.yHotspot = (DWORD)hotSpot.y;
        iconinfo.hbmMask = hMonoBitmap;
        iconinfo.hbmColor = hBitmap;

        _data->Cursor.cursor = CreateIconIndirect(&iconinfo);

        DeleteObject(hBitmap);
        DeleteObject(hMonoBitmap);

        // Make sure we notify the message loop to perform the actual cursor update
        SPtr<RenderWindow> primaryWindow = gCoreApplication().GetWindow();
        UINT64 hwnd;
        primaryWindow->GetCustomAttribute("WINDOW", &hwnd);

        PostMessage((HWND)hwnd, WM_SETCURSOR, WPARAM((HWND)hwnd), (LPARAM)MAKELONG(HTCLIENT, WM_MOUSEMOVE));
    }

    void Platform::SetIcon(const PixelData& pixelData)
    {
        Vector<Color> pixels = pixelData.GetColors();
        UINT32 width = pixelData.GetWidth();
        UINT32 height = pixelData.GetHeight();

        HBITMAP hBitmap = Win32PlatformUtility::CreateBitmap((Color*)pixels.data(), width, height, false);
        HBITMAP hMonoBitmap = CreateBitmap(width, height, 1, 1, nullptr);

        ICONINFO iconinfo = { 0 };
        iconinfo.fIcon = TRUE;
        iconinfo.xHotspot = 0;
        iconinfo.yHotspot = 0;
        iconinfo.hbmMask = hMonoBitmap;
        iconinfo.hbmColor = hBitmap;

        HICON icon = CreateIconIndirect(&iconinfo);

        DeleteObject(hBitmap);
        DeleteObject(hMonoBitmap);

        // Make sure we notify the message loop to perform the actual cursor update
        SPtr<RenderWindow> primaryWindow = gCoreApplication().GetWindow();
        UINT64 hwnd;
        primaryWindow->GetCustomAttribute("WINDOW", &hwnd);

        PostMessage((HWND)hwnd, WM_SETICON, WPARAM(ICON_BIG), (LPARAM)icon);
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
    { }

    void Platform::ShutDown()
    { }

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

            if (msg.message == WM_QUIT)
            {
                PlatformUtility::Terminate(true); // Program don't want to stop sometimes, we force it
                break;
            }
        }
    }

    /**
     * Converts a virtual key code into an input command, if possible. Returns true if conversion was done.
     *
     * @param[in]	virtualKeyCode	Virtual key code to try to translate to a command.
     * @param[out]	command			Input command. Only valid if function returns true.
     */
    bool GetCommand(unsigned int virtualKeyCode, InputCommandType& command)
    {
        bool isShiftPressed = GetAsyncKeyState(VK_SHIFT);
        
        switch (virtualKeyCode)
        {
        case VK_LEFT:
            command = isShiftPressed ? InputCommandType::SelectLeft : InputCommandType::CursorMoveLeft;
            return true;
        case VK_RIGHT:
            command = isShiftPressed ? InputCommandType::SelectRight : InputCommandType::CursorMoveRight;
            return true;
        case VK_UP:
            command = isShiftPressed ? InputCommandType::SelectUp : InputCommandType::CursorMoveUp;
            return true;
        case VK_DOWN:
            command = isShiftPressed ? InputCommandType::SelectDown : InputCommandType::CursorMoveDown;
            return true;
        case VK_ESCAPE:
            command = InputCommandType::Escape;
            return true;
        case VK_RETURN:
            command = isShiftPressed ? InputCommandType::Return : InputCommandType::Confirm;
            return true;
        case VK_BACK:
            command = InputCommandType::Backspace;
            return true;
        case VK_DELETE:
            command = InputCommandType::Delete;
            return true;
        case VK_TAB:
            command = InputCommandType::Tab;
            return true;
        }

        return false;
    }

    LRESULT CALLBACK Win32Platform::_win32WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if (uMsg == WM_CREATE)
        { // Store pointer to Win32Window in user data area
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
                if (LOWORD(wParam) == WA_INACTIVE)
                {
#if TE_DEBUG_MODE == 0
                    gCoreApplication().Pause(true);
                    gTime().Stop();
#endif
                }
                else
                {
#if TE_DEBUG_MODE == 0
                    gCoreApplication().Pause(false);
                    gTime().Start();
#endif
                }

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
                if (!win->GetProperties().HasFocus)
                    win->NotifyWindowEvent(WindowEventType::FocusReceived);

                return 0;
            }
            case WM_KILLFOCUS:
            {
                if (win->GetProperties().HasFocus)
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
                {
                    win->NotifyWindowEvent(WindowEventType::Maximized);
#if TE_DEBUG_MODE == 0
                    gCoreApplication().Pause(false);
                    gTime().Start();
#endif
                }
                else if (wParam == SIZE_MINIMIZED)
                {
                    win->NotifyWindowEvent(WindowEventType::Minimized);
#if TE_DEBUG_MODE == 0
                    gCoreApplication().Pause(true);
                    gTime().Stop();
#endif
                }
                else if (wParam == SIZE_RESTORED)
                {
                    win->NotifyWindowEvent(WindowEventType::Restored);
#if TE_DEBUG_MODE == 0
                    gCoreApplication().Pause(false);
                    gTime().Start();
#endif
                }

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
                PostMessage(NULL, WM_QUIT, 0, 0);
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
                if (!OnKeyDown.Empty())
                    OnKeyDown((UINT32)wParam);

                InputCommandType command = InputCommandType::Backspace;
                if(GetCommand((unsigned int)wParam, command))
                {
                    if(!OnInputCommand.Empty())
                        OnInputCommand(command);

                    return 0;
                }

                break;
            }
            case WM_SYSKEYUP:
            case WM_KEYUP:
            {
                if (!OnKeyUp.Empty())
                    OnKeyUp((UINT32)wParam);

                break;
            }
            case WM_CHAR:
            {
                // Ignore rarely used special command characters, usually triggered by ctrl+key
                // combinations. (We want to keep ctrl+key free for shortcuts instead)
                if (wParam <= 23)
                    break;

                // Ignore shortcut key combinations
                //if (GetAsyncKeyState(VK_CONTROL) != 0 || GetAsyncKeyState(VK_MENU) != 0)
                //    break;

                switch (wParam)
                {
                    case VK_ESCAPE:
                        break;
                    default: // displayable character 
                    {
                        UINT32 finalChar = (UINT32)wParam;
                        if ((lParam & 0x80000000) == 0 && !OnCharInput.Empty()) //We filter "key up" events
                        {
                            OnCharInput(finalChar);
                        }

                        return 0;
                    }
                }

                break;
            }
            case WM_TE_SETCAPTURE:
                SetCapture(hWnd);
                break;
            case WM_DPICHANGED:
                // TODO
                break;
            case WM_TE_RELEASECAPTURE:
                ReleaseCapture();
                break;
            case WM_QUIT:
                PostQuitMessage(0);
                return 0;
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
            case WM_CAPTURECHANGED:
                if (!OnMouseCaptureChanged.Empty())
                    OnMouseCaptureChanged();
                return 0;
        }

        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}