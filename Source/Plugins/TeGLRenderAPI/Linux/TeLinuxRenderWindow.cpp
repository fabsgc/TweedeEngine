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

    void LinuxRenderWindow::Update()
    {
        //TODO
    }

    void LinuxRenderWindow::Initialize()
    {
        /*
        XVisualInfo visualInfoTempl = {};
		visualInfoTempl.screen = XDefaultScreen(LinuxPlatform::GetXDisplay());
		visualInfoTempl.depth = 24;
		visualInfoTempl.c_class = TrueColor;

		int32_t numVisuals;
		XVisualInfo* visualInfo = XGetVisualInfo(LinuxPlatform::GetXDisplay(),
				VisualScreenMask | VisualDepthMask | VisualClassMask, &visualInfoTempl, &numVisuals);
        */

        WINDOW_DESC windowDesc;
		windowDesc.X = _desc.Left;
		windowDesc.Y = _desc.Top;
		windowDesc.Width = _desc.Mode.GetWidth();
		windowDesc.Height = _desc.Mode.GetHeight();
		windowDesc.Title = _desc.Title;
		windowDesc.ShowDecorations = _desc.ShowTitleBar;
		windowDesc.AllowResize = _desc.AllowResize;
        //windowDesc.VisualInfo = visualInfo;
        windowDesc.Screen = 0;

        _window = te_new<LinuxWindow>(windowDesc);

		_properties.Width = _window->GetWidth();
		_properties.Height = _window->GetHeight();
		_properties.Top = _window->GetTop();
		_properties.Left = _window->GetLeft();
        _properties.IsFullScreen = _desc.Fullscreen;

        if(_desc.Fullscreen)
        {
			SetFullscreen(_desc.Mode);
        }

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
			*window = _window->_getXWindow();
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
}