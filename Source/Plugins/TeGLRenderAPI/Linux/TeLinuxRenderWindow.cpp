#include "TeLinuxRenderWindow.h"
#include "Private/Linux/TeLinuxPlatform.h"

namespace te
{
    LinuxRenderWindow::LinuxRenderWindow(const RENDER_WINDOW_DESC& desc)
        : RenderWindow(desc)
        , _window(nullptr)
    {
    }

    LinuxRenderWindow::~LinuxRenderWindow()
    {
        if(_window != nullptr)
        {
            te_delete(_window);
            _window = nullptr;
        }
    }

    void LinuxRenderWindow::Initialize()
    {
        
        XVisualInfo visualInfoTempl = {};
        visualInfoTempl.screen = XDefaultScreen(LinuxPlatform::GetXDisplay());
        visualInfoTempl.depth = 24;
        visualInfoTempl.c_class = TrueColor;

        int32_t numVisuals;
        XVisualInfo* visualInfo = XGetVisualInfo(LinuxPlatform::GetXDisplay(),
                VisualScreenMask | VisualDepthMask | VisualClassMask, &visualInfoTempl, &numVisuals);

        //GLVisualConfig visualConfig = FindBestVisual(LinuxPlatform::GetXDisplay(), _desc.DepthBuffer, _desc.MultisampleCount, _desc.Gamma);

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

        _properties.MultisampleCount = 4;
        _properties.IsWindow = true;

        XWindowAttributes windowAttributes;
        XGetWindowAttributes(LinuxPlatform::GetXDisplay(), _window->GetXWindow(), &windowAttributes);

        if(_desc.Fullscreen)
        {
            SetFullscreen(_desc.Mode);
        }

        // TODO
    }

    void LinuxRenderWindow::InitializeGui()
    { 
        // TODO
    }

    void LinuxRenderWindow::GetCustomAttribute(const String& name, void* pData) const
    {
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
        //TODO
    }

    void LinuxRenderWindow::Resize(UINT32 width, UINT32 height)
    {
        //TODO
    }

    void LinuxRenderWindow::SetHidden(bool hidden)
    {
        //TODO
    }

    void LinuxRenderWindow::SetActive(bool state)
    {
        //TODO
    }

    void LinuxRenderWindow::Minimize()
    {
        //TODO
    }

    void LinuxRenderWindow::Maximize()
    {
        //TODO
    }

    void LinuxRenderWindow::Restore()
    {
        //TODO
    }

    void LinuxRenderWindow::SetFullscreen(UINT32 width, UINT32 height, float refreshRate, UINT32 monitorIdx)
    {
        //TODO
    }

    void LinuxRenderWindow::SetFullscreen(const VideoMode& videoMode)
    {
        //TODO
    }

    void LinuxRenderWindow::SetWindowed(UINT32 width, UINT32 height)
    {
        //TODO
    }

    Vector2I LinuxRenderWindow::ScreenToWindowPos(const Vector2I& screenPos) const
    {
        //TODO
        return Vector2I();
    }

    Vector2I LinuxRenderWindow::WindowToScreenPos(const Vector2I& windowPos) const
    {
        //TODO
        return Vector2I();
    }

    void LinuxRenderWindow::WindowMovedOrResized()
    {
        //TODO
    }

    GLVisualConfig LinuxRenderWindow::FindBestVisual(::Display* display, bool depthStencil, UINT32 multisample, bool srgb)
    {
        GLVisualConfig output;

        // TODO

        /*INT32 VISUAL_ATTRIBS[] =
        {
            GLX_X_RENDERABLE, 		True,
            GLX_DRAWABLE_TYPE, 		GLX_WINDOW_BIT,
            GLX_RENDER_TYPE,		GLX_RGBA_BIT,
            GLX_X_VISUAL_TYPE,		GLX_TRUE_COLOR,
            GLX_RED_SIZE,			8,
            GLX_GREEN_SIZE,			8,
            GLX_BLUE_SIZE,			8,
            GLX_ALPHA_SIZE,			8,
            GLX_DOUBLEBUFFER,		True,
            GLX_DEPTH_SIZE,			depthStencil ? 24 : 0,
            GLX_STENCIL_SIZE,		depthStencil ? 8 : 0,
            GLX_SAMPLE_BUFFERS,		multisample > 1 ? 1 : 0,
            0
        };

        INT32 numConfigs;
        GLXFBConfig* configs = glXChooseFBConfig(display, DefaultScreen(display), VISUAL_ATTRIBS, &numConfigs);
        GLVisualCapabilities* caps = (GLVisualCapabilities*)te_allocate(sizeof(GLVisualCapabilities) * numConfigs);

        XVisualInfo* visualInfo = glXGetVisualFromFBConfig(display, configs[0]);

        output.VisualInfo = *visualInfo;
        output.Caps = caps[0];

        // If we have several configs found, we simply take the first one
        XFree(configs);
        XFree(visualInfo);

        te_delete(caps);*/

        return output;
    }

    void LinuxRenderWindow::SetTitle(const String& title)
    { }
}
