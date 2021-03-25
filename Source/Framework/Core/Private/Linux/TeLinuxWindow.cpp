#include "Private/Linux/TeLinuxWindow.h"
#include "Private/Linux/TeLinuxPlatform.h"
#include "Image/TePixelUtil.h"
#include "Image/TeColor.h"

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
        _data->X = x;
        _data->Y = y;

        XMoveWindow(LinuxPlatform::GetXDisplay(), _data->XWindow, x, y);
    }

    void LinuxWindow::SetIcon(const PixelData& data)
    {
        constexpr UINT32 WIDTH = 128;
        constexpr UINT32 HEIGHT = 128;

        PixelData resizedData(WIDTH, HEIGHT, 1, PF_RGBA8);
        resizedData.AllocateInternalBuffer();

        PixelUtil::Scale(data, resizedData);

        ::Display* display = LinuxPlatform::GetXDisplay();

        // Set icon the old way using IconPixmapHint.
        Pixmap iconPixmap = LinuxPlatform::CreatePixmap(resizedData, (UINT32)XDefaultDepth(display,
                XDefaultScreen(display)));

        XWMHints* hints = XAllocWMHints();
        hints->flags = IconPixmapHint;
        hints->icon_pixmap = iconPixmap;

        XSetWMHints(display, _data->XWindow, hints);

        XFree(hints);
        XFreePixmap(display, iconPixmap);

        // Also try to set _NET_WM_ICON for modern window managers.
        // Using long because the spec for XChangeProperty states that format size of 32 = long (this means for 64-bit it
        // is padded in upper 4 bytes)
        Vector<long> wmIconData(2 + WIDTH * HEIGHT, 0);
        wmIconData[0] = WIDTH;
        wmIconData[1] = HEIGHT;
        for (UINT32 y = 0; y < HEIGHT; y++)
            for (UINT32 x = 0; x < WIDTH; x++)
                wmIconData[y * WIDTH + x + 2] = resizedData.GetColorAt(x, y).GetAsBGRA();

        Atom iconAtom = XInternAtom(display, "_NET_WM_ICON", False);
        Atom cardinalAtom = XInternAtom(display, "CARDINAL", False);
        XChangeProperty(display, _data->XWindow, iconAtom, cardinalAtom, 32, PropModeReplace,
                (const unsigned char*) wmIconData.data(), wmIconData.size());

        XFlush(display);
    }

    void LinuxWindow::Resize(UINT32 width, UINT32 height)
    {
        // If resize is disabled on WM level, we need to force it
        if(_data->ResizeDisabled)
        {
            XSizeHints hints;
            hints.flags = PMinSize | PMaxSize;

            hints.min_height = height;
            hints.max_height = height;

            hints.min_width = width;
            hints.max_width = width;

            XSetNormalHints(LinuxPlatform::GetXDisplay(), _data->XWindow, &hints);
        }

        _data->Width = width;
        _data->Height = height;

        XResizeWindow(LinuxPlatform::GetXDisplay(), _data->XWindow, width, height);
    }

    void LinuxWindow::Hide()
    {
        XUnmapWindow(LinuxPlatform::GetXDisplay(), _data->XWindow);
    }

    void LinuxWindow::Show()
    {
        XMapWindow(LinuxPlatform::GetXDisplay(), _data->XWindow);
        XMoveResizeWindow(LinuxPlatform::GetXDisplay(), _data->XWindow, _data->X, _data->Y, _data->Width, _data->Height);
    }

    void LinuxWindow::Maximize()
    {
        Maximize(true);
    }

    void LinuxWindow::Minimize()
    {
        Minimize(true);
    }

    void LinuxWindow::Restore()
    {
        if(IsMaximized())
            Maximize(false);
        else if(IsMinimized())
            Minimize(false);
    }

    INT32 LinuxWindow::GetLeft() const
    {
        INT32 x, y;
        ::Window child;
        XTranslateCoordinates(LinuxPlatform::GetXDisplay(), _data->XWindow, DefaultRootWindow(LinuxPlatform::GetXDisplay()),
                0, 0, &x, &y, &child);

        return x;
    }

    INT32 LinuxWindow::GetTop() const
    {
        INT32 x, y;
        ::Window child;
        XTranslateCoordinates(LinuxPlatform::GetXDisplay(), _data->XWindow, DefaultRootWindow(LinuxPlatform::GetXDisplay()),
                0, 0, &x, &y, &child);

        return y;
    }

    UINT32 LinuxWindow::GetWidth() const
    {
        XWindowAttributes xwa;
        XGetWindowAttributes(LinuxPlatform::GetXDisplay(), _data->XWindow, &xwa);

        return (UINT32)xwa.width;
    }

    UINT32 LinuxWindow::GetHeight() const
    {
        XWindowAttributes xwa;
        XGetWindowAttributes(LinuxPlatform::GetXDisplay(), _data->XWindow, &xwa);

        return (UINT32)xwa.height;
    }

    Vector2I LinuxWindow::WindowToScreenPos(const Vector2I& windowPos) const
    {
        Vector2I screenPos;

        ::Window child;
        XTranslateCoordinates(LinuxPlatform::GetXDisplay(), _data->XWindow, DefaultRootWindow(LinuxPlatform::GetXDisplay()),
                windowPos.x, windowPos.y, &screenPos.x, &screenPos.y, &child);

        return screenPos;
    }

    Vector2I LinuxWindow::ScreenToWindowPos(const Vector2I& screenPos) const
    {
        Vector2I windowPos;

        ::Window child;
        XTranslateCoordinates(LinuxPlatform::GetXDisplay(), DefaultRootWindow(LinuxPlatform::GetXDisplay()), _data->XWindow,
                screenPos.x, screenPos.y, &windowPos.x, &windowPos.y, &child);

        return windowPos;
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

    bool LinuxWindow::IsMaximized() const
    {
        Atom wmState = XInternAtom(LinuxPlatform::GetXDisplay(), "_NET_WM_STATE", False);
        Atom type;
        INT32 format;
        uint64_t length;
        uint64_t remaining;
        uint8_t* data = nullptr;

        INT32 result = XGetWindowProperty(LinuxPlatform::GetXDisplay(), _data->XWindow, wmState,
            0, 1024, False, XA_ATOM, &type, &format,
            &length, &remaining, &data);

        if (result == Success)
        {
            Atom* atoms = (Atom*)data;
            Atom wmMaxHorz = XInternAtom(LinuxPlatform::GetXDisplay(), "_NET_WM_STATE_MAXIMIZED_HORZ", False);
            Atom wmMaxVert = XInternAtom(LinuxPlatform::GetXDisplay(), "_NET_WM_STATE_MAXIMIZED_VERT", False);

            bool foundHorz = false;
            bool foundVert = false;
            for (UINT32 i = 0; i < length; i++)
            {
                if (atoms[i] == wmMaxHorz)
                    foundHorz = true;
                if (atoms[i] == wmMaxVert)
                    foundVert = true;

                if (foundVert && foundHorz)
                    return true;
            }

            XFree(atoms);
        }

        return false;
    }

    bool LinuxWindow::IsMinimized()
    {
        Atom wmState = XInternAtom(LinuxPlatform::GetXDisplay(), "WM_STATE", True);
        Atom type;
        INT32 format;
        uint64_t length;
        uint64_t remaining;
        uint8_t* data = nullptr;

        INT32 result = XGetWindowProperty(LinuxPlatform::GetXDisplay(), _data->XWindow, wmState,
                0, 1024, False, AnyPropertyType, &type, &format,
                &length, &remaining, &data);

        if(result == Success)
        {
            long* state = (long*) data;
            if(state[0] == WM_IconicState)
                return true;
        }

        return false;
    }

    void LinuxWindow::Maximize(bool enable)
    {
        Atom wmState = XInternAtom(LinuxPlatform::GetXDisplay(), "_NET_WM_STATE", False);
        Atom wmMaxHorz = XInternAtom(LinuxPlatform::GetXDisplay(), "_NET_WM_STATE_MAXIMIZED_HORZ", False);
        Atom wmMaxVert = XInternAtom(LinuxPlatform::GetXDisplay(), "_NET_WM_STATE_MAXIMIZED_VERT", False);

        XEvent xev;
        memset(&xev, 0, sizeof(xev));
        xev.type = ClientMessage;
        xev.xclient.window = _data->XWindow;
        xev.xclient.message_type = wmState;
        xev.xclient.format = 32;
        xev.xclient.data.l[0] = enable ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE;
        xev.xclient.data.l[1] = wmMaxHorz;
        xev.xclient.data.l[2] = wmMaxVert;

        XSendEvent(LinuxPlatform::GetXDisplay(), DefaultRootWindow(LinuxPlatform::GetXDisplay()), False,
                SubstructureRedirectMask | SubstructureNotifyMask, &xev);
    }

    void LinuxWindow::Minimize(bool enable)
    {
        XEvent xev;
        Atom wmChange = XInternAtom(LinuxPlatform::GetXDisplay(), "WM_CHANGE_STATE", False);

        memset(&xev, 0, sizeof(xev));
        xev.type = ClientMessage;
        xev.xclient.window = _data->XWindow;
        xev.xclient.message_type = wmChange;
        xev.xclient.format = 32;
        xev.xclient.data.l[0] = enable ? WM_IconicState : WM_NormalState;

        XSendEvent(LinuxPlatform::GetXDisplay(), DefaultRootWindow(LinuxPlatform::GetXDisplay()), False,
                SubstructureRedirectMask | SubstructureNotifyMask, &xev);
    }
}
