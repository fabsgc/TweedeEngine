#include "TeWin32RenderWindow.h"

#include "Private/WIn32/TeWin32Platform.h"
#include "Win32/TeWin32VideoModeInfo.h"
#include "Win32/TeWin32GLSupport.h"
#include "Win32/TeWin32Context.h"
#include "Manager/TeGuiManager.h"
#include "Gui/TeGuiAPI.h"

GLenum GLEWAPIENTRY wglewContextInit(te::GLSupport* glSupport);

namespace te
{
    struct GuiAPIData
    { };

    Win32RenderWindow::Win32RenderWindow(const RENDER_WINDOW_DESC& desc, Win32GLSupport& glsupport)
        : RenderWindow(desc)
        , _window(nullptr)
        , _deviceName(nullptr)
        , _displayFrequency(0)
        , _HDC(nullptr)
        , _GLSupport(glsupport)
        , _context(nullptr)
    { 
        _GLSupport._notifyWindowCreated(this);
    }

    Win32RenderWindow::~Win32RenderWindow()
    {
        if (_window != nullptr)
        {
            ReleaseDC(_window->GetHWnd(), _HDC);

            _window->Destroy();
            te_delete(_window);
        }

        _HDC = nullptr;

        if (_deviceName != nullptr)
        {
            te_free(_deviceName);
            _deviceName = nullptr;
        }
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

        _properties.MultisampleCount = _desc.MultisampleCount;
        _properties.IsWindow = true;

        _properties.IsFullScreen = _desc.Fullscreen;
        _displayFrequency = Math::RoundToInt(_desc.Mode._refreshRate);

        if (_properties.IsFullScreen)
        {
            DEVMODE displayDeviceMode;

            memset(&displayDeviceMode, 0, sizeof(displayDeviceMode));
            displayDeviceMode.dmSize = sizeof(DEVMODE);
            displayDeviceMode.dmBitsPerPel = 32;
            displayDeviceMode.dmPelsWidth = _properties.Width;
            displayDeviceMode.dmPelsHeight = _properties.Height;
            displayDeviceMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

            if (_displayFrequency)
            {
                displayDeviceMode.dmDisplayFrequency = _displayFrequency;
                displayDeviceMode.dmFields |= DM_DISPLAYFREQUENCY;

                if (ChangeDisplaySettingsEx(_deviceName, &displayDeviceMode, NULL, CDS_FULLSCREEN | CDS_TEST, NULL) != DISP_CHANGE_SUCCESSFUL)
                {
                    TE_ASSERT_ERROR(false, "ChangeDisplaySettings with user display frequency failed.");
                }
            }

            if (ChangeDisplaySettingsEx(_deviceName, &displayDeviceMode, NULL, CDS_FULLSCREEN, NULL) != DISP_CHANGE_SUCCESSFUL)
            {
                TE_ASSERT_ERROR(false, "ChangeDisplaySettings failed.");
            }
        }

        _HDC = GetDC(_window->GetHWnd());

        int testMultisample = _properties.MultisampleCount;
        bool testHwGamma = _desc.Gamma;
        bool formatOk = _GLSupport.SelectPixelFormat(_HDC, 32, testMultisample, testHwGamma, _desc.DepthBuffer);
        if (!formatOk)
        {
            if (_properties.MultisampleCount > 0)
            {
                // Try without multisampling
                testMultisample = 0;
                formatOk = _GLSupport.SelectPixelFormat(_HDC, 32, testMultisample, testHwGamma, _desc.DepthBuffer);
            }

            if (!formatOk && _desc.Gamma)
            {
                // Try without sRGB
                testHwGamma = false;
                testMultisample = _properties.MultisampleCount;
                formatOk = _GLSupport.SelectPixelFormat(_HDC, 32, testMultisample, testHwGamma, _desc.DepthBuffer);
            }

            if (!formatOk && _desc.Gamma && (_properties.MultisampleCount > 0))
            {
                // Try without both
                testHwGamma = false;
                testMultisample = 0;
                formatOk = _GLSupport.SelectPixelFormat(_HDC, 32, testMultisample, testHwGamma, _desc.DepthBuffer);
            }

            if (!formatOk)
                TE_ASSERT_ERROR(false, "Failed selecting pixel format.");
        }

        // Record what gamma option we used in the end
        // this will control enabling of sRGB state flags when used
        _properties.HWGamma = testHwGamma;
        _properties.MultisampleCount = testMultisample;

        _context = _GLSupport.CreateContext(_HDC, nullptr);

        _GLSupport._notifyWindowCreated(this);

        RenderWindow::Initialize();
    }

    void Win32RenderWindow::InitializeGui()
    {
        GuiAPIData data;

        SPtr<GuiAPI> guiAPI = GuiManager::Instance().GetGui();
        guiAPI->Initialize((void*)&data);
    }

    void Win32RenderWindow::GetCustomAttribute(const String& name, void* pData) const
    {
        if(name == "GLCONTEXT")
        {
            SPtr<GLContext>* contextPtr = static_cast<SPtr<GLContext>*>(pData);
            *contextPtr = _context;
            return;
        }
        else if (name == "WINDOW")
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

    void Win32RenderWindow::SetVSync(bool enabled)
    {
        if(enabled)
            wglSwapIntervalEXT(1);
        else
            wglSwapIntervalEXT(0);
        TE_CHECK_GL_ERROR();

        _properties.VSync = enabled;
    }

    void Win32RenderWindow::SetFullscreen(UINT32 width, UINT32 height, float refreshRate, UINT32 monitorIdx)
    {
        const Win32VideoModeInfo& videoModeInfo = static_cast<const Win32VideoModeInfo&>(RenderAPI::Instance().GetVideoModeInfo());
        UINT32 numOutputs = videoModeInfo.GetNumOutputs();
        if (numOutputs == 0)
            return;

        UINT32 actualMonitorIdx = std::min(monitorIdx, numOutputs - 1);
        const Win32VideoOutputInfo& outputInfo = static_cast<const Win32VideoOutputInfo&>(videoModeInfo.GetOutputInfo(actualMonitorIdx));

        _displayFrequency = Math::RoundToInt(refreshRate);
        _properties.IsFullScreen = true;

        DEVMODE displayDeviceMode;

        memset(&displayDeviceMode, 0, sizeof(displayDeviceMode));
        displayDeviceMode.dmSize = sizeof(DEVMODE);
        displayDeviceMode.dmBitsPerPel = 32;
        displayDeviceMode.dmPelsWidth = width;
        displayDeviceMode.dmPelsHeight = height;
        displayDeviceMode.dmDisplayFrequency = _displayFrequency;
        displayDeviceMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

        HMONITOR hMonitor = outputInfo.GetMonitorHandle();
        MONITORINFOEX monitorInfo;

        memset(&monitorInfo, 0, sizeof(MONITORINFOEX));
        monitorInfo.cbSize = sizeof(MONITORINFOEX);
        GetMonitorInfo(hMonitor, &monitorInfo);

        if (ChangeDisplaySettingsEx(monitorInfo.szDevice, &displayDeviceMode, NULL, CDS_FULLSCREEN, NULL) != DISP_CHANGE_SUCCESSFUL)
        {
            TE_ASSERT_ERROR(false, "ChangeDisplaySettings failed");
        }

        _properties.Top = monitorInfo.rcMonitor.top;
        _properties.Left = monitorInfo.rcMonitor.left;
        _properties.Width = width;
        _properties.Height = height;

        SetWindowLong(_window->GetHWnd(), GWL_STYLE, WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
        SetWindowLong(_window->GetHWnd(), GWL_EXSTYLE, 0);

        SetWindowPos(_window->GetHWnd(), HWND_TOP, _properties.Left, _properties.Top, width, height, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

        WindowMovedOrResized();
    }

    void Win32RenderWindow::SetFullscreen(const VideoMode& mode)
    {
        SetFullscreen(mode._width, mode._height, mode._refreshRate, mode._outputIdx);
    }

    void Win32RenderWindow::SetWindowed(UINT32 width, UINT32 height)
    {
        if (!_properties.IsFullScreen)
            return;

        _properties.IsFullScreen = false;
        _properties.Width = width;
        _properties.Height = height;

        // Drop out of fullscreen
        ChangeDisplaySettingsEx(_deviceName, NULL, NULL, 0, NULL);

        UINT32 winWidth = width;
        UINT32 winHeight = height;

        RECT rect;
        SetRect(&rect, 0, 0, winWidth, winHeight);

        AdjustWindowRect(&rect, _window->GetStyle(), false);
        winWidth = rect.right - rect.left;
        winHeight = rect.bottom - rect.top;

        // Deal with centering when switching down to smaller resolution
        HMONITOR hMonitor = MonitorFromWindow(_window->GetHWnd(), MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo;
        memset(&monitorInfo, 0, sizeof(MONITORINFO));
        monitorInfo.cbSize = sizeof(MONITORINFO);
        GetMonitorInfo(hMonitor, &monitorInfo);

        LONG screenw = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
        LONG screenh = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

        INT32 left = screenw > INT32(winWidth) ? ((screenw - INT32(winWidth)) / 2) : 0;
        INT32 top = screenh > INT32(winHeight) ? ((screenh - INT32(winHeight)) / 2) : 0;

        SetWindowLong(_window->GetHWnd(), GWL_STYLE, _window->GetStyle() | WS_VISIBLE);
        SetWindowLong(_window->GetHWnd(), GWL_EXSTYLE, _window->GetStyleEx());
        SetWindowPos(_window->GetHWnd(), HWND_NOTOPMOST, left, top, winWidth, winHeight,
            SWP_DRAWFRAME | SWP_FRAMECHANGED | SWP_NOACTIVATE);

        WindowMovedOrResized();
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

        if (_properties.IsFullScreen)
        {
            _properties.Top = _window->GetTop();
            _properties.Left = _window->GetLeft();
            _properties.Width = _window->GetWidth();
            _properties.Height = _window->GetHeight();
        }
    }

    void Win32RenderWindow::SwapBuffers()
    {
        ::SwapBuffers(_HDC);
    }

    void Win32RenderWindow::SetTitle(const String& title)
    {
        SetWindowText(GetHWnd(), title.c_str());
    }
}
