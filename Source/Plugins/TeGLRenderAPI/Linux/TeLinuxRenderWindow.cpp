#include "TeLinuxRenderWindow.h"
#include "Private/Linux/TeLinuxPlatform.h"
#include "Manager/TeGuiManager.h"
#include "Gui/TeGuiAPI.h"

namespace te
{
    struct GuiAPIData
    { };

    LinuxRenderWindow::LinuxRenderWindow(const RENDER_WINDOW_DESC& desc, LinuxGLSupport& glsupport)
        : RenderWindow(desc)
        , _window(nullptr)
        , _GLSupport(glsupport)
        , _context(nullptr)
    { }

    LinuxRenderWindow::~LinuxRenderWindow()
    {
        if(_window != nullptr)
        {
            LinuxPlatform::LockX();

            te_delete(_window);
            _window = nullptr;

            LinuxPlatform::UnlockX();
        }
    }

    void LinuxRenderWindow::Initialize()
    {
        LinuxPlatform::LockX();

        XVisualInfo visualInfoTempl = {};
        visualInfoTempl.screen = XDefaultScreen(LinuxPlatform::GetXDisplay());
        visualInfoTempl.depth = 24;
        visualInfoTempl.c_class = TrueColor;

        int32_t numVisuals;
        XVisualInfo* visualInfo = XGetVisualInfo(LinuxPlatform::GetXDisplay(),
                VisualScreenMask | VisualDepthMask | VisualClassMask, &visualInfoTempl, &numVisuals);

        GLVisualConfig visualConfig = _GLSupport.FindBestVisual(LinuxPlatform::GetXDisplay(), _desc.DepthBuffer,
                _desc.MultisampleCount, _desc.Gamma);

        WINDOW_DESC windowDesc;
        windowDesc.X = _desc.Left;
        windowDesc.Y = _desc.Top;
        windowDesc.Width = _desc.Mode.GetWidth();
        windowDesc.Height = _desc.Mode.GetHeight();
        windowDesc.Title = _desc.Title;
        windowDesc.ShowDecorations = _desc.ShowTitleBar;
        windowDesc.AllowResize = _desc.AllowResize;
        windowDesc.VisualInfo = *visualInfo;
        windowDesc.Screen = 0;

        _window = te_new<LinuxWindow>(windowDesc);
        _window->SetRenderWindow(this);

        _properties.Width = _window->GetWidth();
        _properties.Height = _window->GetHeight();
        _properties.Top = _window->GetTop();
        _properties.Left = _window->GetLeft();
        _properties.IsFullScreen = _desc.Fullscreen;

        //_properties.HWGamma = visualConfig.caps.srgb;
        //_properties.MultisampleCount = visualConfig.caps.numSamples;

        _properties.MultisampleCount = 4;
        _properties.IsWindow = true;

        XWindowAttributes windowAttributes;
        XGetWindowAttributes(LinuxPlatform::GetXDisplay(), _window->GetXWindow(), &windowAttributes);

        XVisualInfo requestVI;
        requestVI.screen = windowDesc.Screen;
        requestVI.visualid = XVisualIDFromVisual(windowAttributes.visual);

        LinuxPlatform::UnlockX(); // Calls below have their own locking mechanisms

        _context = _GLSupport.CreateContext(LinuxPlatform::GetXDisplay(), requestVI);

        if(_desc.Fullscreen)
            SetFullscreen(_desc.Mode);

        RenderWindow::Initialize();
    }

    void LinuxRenderWindow::InitializeGui()
    {
        GuiAPIData data;

        SPtr<GuiAPI> guiAPI = GuiManager::Instance().GetGui();
        guiAPI->Initialize((void*)&data);
    }

    void LinuxRenderWindow::GetCustomAttribute(const String& name, void* pData) const
    {
        if(name == "GLCONTEXT")
        {
            SPtr<GLContext>* contextPtr = static_cast<SPtr<GLContext>*>(pData);
            *contextPtr = _context;
            return;
        }
        if(name == "LINUX_WINDOW")
        {
            LinuxWindow** window = (LinuxWindow**)pData;
            *window = _window;
            return;
        }
        else if(name == "WINDOW")
        {
            ::Window* window = (::Window*)pData;
            *window = _window->GetXWindow();
            return;
        }

        RenderWindow::GetCustomAttribute(name, pData);
    }

    void LinuxRenderWindow::Move(INT32 left, INT32 top)
    {
        // TODO
    }

    void LinuxRenderWindow::Resize(UINT32 width, UINT32 height)
    {
        // TODO
    }

    void LinuxRenderWindow::SetHidden(bool hidden)
    {
        // TODO
    }

    void LinuxRenderWindow::SetActive(bool state)
    {
        // TODO
    }

    void LinuxRenderWindow::Minimize()
    {
        // TODO
    }

    void LinuxRenderWindow::Maximize()
    {
        // TODO
    }

    void LinuxRenderWindow::Restore()
    {
        // TODO
    }

    void LinuxRenderWindow::SetVSync(bool enabled)
    {
        // TODO
    }

    void LinuxRenderWindow::SetFullscreen(UINT32 width, UINT32 height, float refreshRate, UINT32 monitorIdx)
    {
        // TODO
    }

    void LinuxRenderWindow::SetFullscreen(const VideoMode& videoMode)
    {
        // TODO
    }

    void LinuxRenderWindow::SetWindowed(UINT32 width, UINT32 height)
    {
        // TODO
    }

    Vector2I LinuxRenderWindow::ScreenToWindowPos(const Vector2I& screenPos) const
    {
        // TODO
        return Vector2I();
    }

    Vector2I LinuxRenderWindow::WindowToScreenPos(const Vector2I& windowPos) const
    {
        // TODO
        return Vector2I();
    }

    void LinuxRenderWindow::WindowMovedOrResized()
    {
        // TODO
    }

    void LinuxRenderWindow::SetTitle(const String& title)
    { }
}
