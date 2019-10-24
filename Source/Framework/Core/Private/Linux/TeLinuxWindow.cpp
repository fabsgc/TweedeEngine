#include "Private/Linux/TeLinuxWindow.h"
#include "Private/Linux/TeLinuxPlatform.h"

#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>
#include <X11/Xutil.h>
#include <X11/Xlib.h>

namespace te
{
    struct LinuxWindow::Pimpl
	{
		::Window XWindow = 0;
		te::RenderWindow* RenderWindow = nullptr;

		INT32 X, Y;
		UINT32 Width, Height;
		bool HasTitleBar = true;
		bool ResizeDisabled = false;
		bool IsExternal = false;
    };

    LinuxWindow::LinuxWindow(const WINDOW_DESC &desc)
	{
		_data = te_new<Pimpl>();

		::Display* display = LinuxPlatform::GetXDisplay();

		// Find the screen of the chosen monitor, as well as its current dimensions
		INT32 screen = XDefaultScreen(display);
		UINT32 outputIdx = 0;

		RROutput primaryOutput = XRRGetOutputPrimary(display, RootWindow(display, screen));
		INT32 monitorX = 0;
		INT32 monitorY = 0;
		UINT32 monitorWidth = 0;
		UINT32 monitorHeight = 0;

		INT32 screenCount = XScreenCount(display);
		for(INT32 i = 0; i < screenCount; i++)
		{
			XRRScreenResources* screenRes = XRRGetScreenResources(display, RootWindow(display, i));

			bool foundMonitor = false;
			for (INT32 j = 0; j < screenRes->noutput; j++)
			{
				XRROutputInfo* outputInfo = XRRGetOutputInfo(display, screenRes, screenRes->outputs[j]);
				if (outputInfo == nullptr || outputInfo->crtc == 0 || outputInfo->connection == RR_Disconnected)
				{
					XRRFreeOutputInfo(outputInfo);
					continue;
				}

				XRRCrtcInfo* crtcInfo = XRRGetCrtcInfo(display, screenRes, outputInfo->crtc);
				if (crtcInfo == nullptr)
				{
					XRRFreeCrtcInfo(crtcInfo);
					XRRFreeOutputInfo(outputInfo);
					continue;
				}

				if(desc.Screen == (UINT32)-1)
				{
					if(screenRes->outputs[j] == primaryOutput)
					{
						foundMonitor = true;
					}
				}
				else
				{
					foundMonitor = outputIdx == desc.Screen;
				}

				if(foundMonitor)
				{
					screen = i;

					monitorX = crtcInfo->x;
					monitorY = crtcInfo->y;
					monitorWidth = crtcInfo->width;
					monitorHeight = crtcInfo->height;

					foundMonitor = true;
					break;
				}
			}

			if(foundMonitor)
			{
				break;
			}
		}

		XSetWindowAttributes attributes;
		attributes.background_pixel = XWhitePixel(display, screen);
		attributes.border_pixel = XBlackPixel(display, screen);
		attributes.background_pixmap = 0;

		attributes.colormap = XCreateColormap(display,
			XRootWindow(display, screen),
			desc.VisualInfo.visual,
			AllocNone);

		// If no position specified, center on the requested monitor
		if (desc.X == -1)
			_data->X = monitorX + (monitorWidth - desc.Width) / 2;
		else if (desc.Screen != (UINT32)-1)
			_data->X = monitorX + desc.X;
		else
			_data->X = desc.X;

		if (desc.Y == -1)
			_data->Y = monitorY + (monitorHeight - desc.Height) / 2;
		else if (desc.Screen != (UINT32)-1)
			_data->Y = monitorY + desc.Y;
		else
			_data->Y = desc.Y;

		_data->Width = desc.Width;
		_data->Height = desc.Height;

		_data->XWindow = XCreateWindow(display,
			XRootWindow(display, screen),
			_data->X, _data->Y,
			_data->Width, _data->Height,
			0, desc.VisualInfo.depth,
			InputOutput, desc.VisualInfo.visual,
			CWBackPixel | CWBorderPixel | CWColormap | CWBackPixmap, &attributes);

		XStoreName(display, _data->XWindow, desc.Title.c_str());

		// Position/size might have (and usually will) get overridden by the WM, so re-apply them
		XSizeHints hints;
		hints.flags = PPosition | PSize;
		hints.x = _data->X;
		hints.y = _data->Y;
		hints.width = _data->Width;
		hints.height = _data->Height;

		if(!desc.AllowResize)
		{
			hints.flags |= PMinSize | PMaxSize;

			hints.min_height = desc.Height;
			hints.max_height = desc.Height;

			hints.min_width = desc.Width;
			hints.max_width = desc.Width;
		}

		XSetNormalHints(display, _data->XWindow, &hints);

		XClassHint* classHint = XAllocClassHint();

		classHint->res_class = (char*)"TweedeFrameworkRedux";
		classHint->res_name = (char*)desc.Title.c_str();

		XSetClassHint(display, _data->XWindow, classHint);
		XFree(classHint);

		long eventMask =
			ExposureMask | FocusChangeMask |
			KeyPressMask | KeyReleaseMask |
			ButtonPressMask | ButtonReleaseMask |
			EnterWindowMask | LeaveWindowMask |
			PointerMotionMask | ButtonMotionMask |
			StructureNotifyMask | PropertyChangeMask |
			SubstructureNotifyMask | SubstructureRedirectMask;

		XSelectInput(display, _data->XWindow, eventMask);

		// Make sure we get the window delete message from WM, so we can clean up ourselves
		Atom atomDeleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", False);
		XSetWMProtocols(display, _data->XWindow, &atomDeleteWindow, 1);

		XMapWindow(display, _data->XWindow);

		_data->HasTitleBar = true;
		_data->ResizeDisabled = !desc.AllowResize;

		XSelectInput(display, _data->XWindow, ExposureMask|ButtonPressMask|KeyPressMask);

    	XMapWindow(display, _data->XWindow);

		LinuxPlatform::RegisterWindow(_data->XWindow, this);
    }

    LinuxWindow::~LinuxWindow()
	{
		if(_data->XWindow != 0)
		{
			XUnmapWindow(LinuxPlatform::GetXDisplay(), _data->XWindow);
			XSync(LinuxPlatform::GetXDisplay(), 0);

			XDestroyWindow(LinuxPlatform::GetXDisplay(), _data->XWindow);
			XSync(LinuxPlatform::GetXDisplay(), 0);

			_data->XWindow = 0;
		}

		XCloseDisplay(LinuxPlatform::GetXDisplay());

		te_delete(_data);
	}

	void LinuxWindow::Move(INT32 x, INT32 y)
	{
		// TODO
	}

	void LinuxWindow::Resize(UINT32 width, UINT32 height)
	{
		// TODO
	}

	void LinuxWindow::Hide()
	{
		// TODO
	}

	void LinuxWindow::Show()
	{
		// TODO
	}

	void LinuxWindow::Maximize()
	{
		// TODO
	}

	void LinuxWindow::Minimize()
	{
		// TODO
	}

	void LinuxWindow::Restore()
	{
		// TODO
	}

	INT32 LinuxWindow::GetLeft() const
	{
		return 0; // TODO
	}

	INT32 LinuxWindow::GetTop() const
	{
		return 0; // TODO
	}

	UINT32 LinuxWindow::GetWidth() const
	{
		return 0; // TODO
	}

	UINT32 LinuxWindow::GetHeight() const
	{
		return 0; // TODO
	}

	Vector2I LinuxWindow::WindowToScreenPos(const Vector2I& windowPos) const
	{
		return Vector2I(); // TODO
	}

	Vector2I LinuxWindow::ScreenToWindowPos(const Vector2I& screenPos) const
	{
		return Vector2I(); // TODO
	}

	::Window LinuxWindow::GetXWindow() const
	{
		return _data->XWindow;
	}

	void LinuxWindow::SetRenderWindow(RenderWindow* renderWindow)
	{
		_data->RenderWindow = renderWindow;
	}

	RenderWindow* LinuxWindow::GetRenderWindow() const
	{
		return _data->RenderWindow;
	}
}