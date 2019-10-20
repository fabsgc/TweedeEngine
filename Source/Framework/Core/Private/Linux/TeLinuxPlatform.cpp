#include "Private/Linux/TeLinuxInput.h"
#include "Private/Linux/TeLinuxPlatform.h"
#include "Private/Linux/TeLinuxWindow.h"
#include "RenderAPI/TeRenderWindow.h"
#include "Math/TeRect2I.h"
#include "TeCoreApplication.h"
#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xcursor/Xcursor.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XInput2.h>
#include <X11/cursorfont.h>

namespace te
{
    Event<void(const Vector2I&, const OSPointerButtonStates&)> Platform::OnCursorMoved;
	Event<void(const Vector2I&, OSMouseButton button, const OSPointerButtonStates&)> Platform::OnCursorButtonPressed;
	Event<void(const Vector2I&, OSMouseButton button, const OSPointerButtonStates&)> Platform::OnCursorButtonReleased;
	Event<void(const Vector2I&, const OSPointerButtonStates&)> Platform::OnCursorDoubleClick;
	Event<void(float)> Platform::OnMouseWheelScrolled;
	Event<void(UINT32)> Platform::OnCharInput;

	Queue<LinuxButtonEvent> LinuxPlatform::ButtonEvents;
	LinuxMouseMotionEvent LinuxPlatform::MouseMotionEvent;

    struct Platform::Pimpl
	{
		::Display* XDisplay = nullptr;
		::Window MainXWindow = 0;
		::Window FullscreenXWindow = 0;

		XIM IM;
		XIC IC;
		::Time LastButtonPressTime;

		// X11 Event handling
		int XInput2Opcode;
		UnorderedMap<String, KeyCode> KeyNameMap; /**< Maps X11 key name (e.g. "TAB") to system-specific X11 KeyCode. */
		Vector<ButtonCode> KeyCodeMap; /**< Maps system-specific X11 KeyCode to Banshee ButtonCode. */

		// Cursor
		::Cursor CurrentCursor = None;
		::Cursor EmptyCursor = None;
		bool IsCursorHidden = false;

		Rect2I CursorClipRect;
		LinuxWindow* CursorClipWindow = nullptr;
		bool CursorClipEnabled = false;
    };

	int X11ErrorHandler(::Display* display, XErrorEvent* event)
	{
		// X11 by default crashes the app on error, even though some errors can be just fine. So we provide our own handler.

		char buffer[256];
		XGetErrorText(display, event->error_code, buffer, sizeof(buffer));
		TE_DEBUG("X11 error: " + String(buffer), __FILE__, __LINE__);

		return 0;
	}

	Platform::Pimpl* Platform::_data = te_new<Platform::Pimpl>();

	static const UINT32 DOUBLE_CLICK_MS = 500;

	Platform::~Platform()
	{ 
	}

	void ApplyCurrentCursor(Platform::Pimpl* data, ::Window window)
	{
		if(data->IsCursorHidden)
		{
			XDefineCursor(data->XDisplay, window, data->EmptyCursor);
		}
		else
		{
			if (data->CurrentCursor != None)
			{
				XDefineCursor(data->XDisplay, window, data->CurrentCursor);
			}
			else
			{
				XUndefineCursor(data->XDisplay, window);
			}
		}
	}

	void UpdateClipBounds(Platform::Pimpl* data, LinuxWindow* window)
	{
		if(!data->CursorClipEnabled || data->CursorClipWindow != window)
			return;

		data->CursorClipRect.x = window->GetLeft();
		data->CursorClipRect.y = window->GetTop();
		data->CursorClipRect.width = window->GetWidth();
		data->CursorClipRect.height = window->GetHeight();
	}

	bool ClipCursor(Platform::Pimpl* data, Vector2I& pos)
	{
		if(!data->CursorClipEnabled)
			return false;

		INT32 clippedX = pos.x - data->CursorClipRect.x;
		INT32 clippedY = pos.y - data->CursorClipRect.y;

		if(clippedX < 0)
			clippedX = 0;
		else if(clippedX >= (INT32)data->CursorClipRect.width)
			clippedX = data->CursorClipRect.width > 0 ? data->CursorClipRect.width - 1 : 0;

		if(clippedY < 0)
			clippedY = 0;
		else if(clippedY >= (INT32)data->CursorClipRect.height)
			clippedY = data->CursorClipRect.height > 0 ? data->CursorClipRect.height - 1 : 0;

		clippedX += data->CursorClipRect.x;
		clippedY += data->CursorClipRect.y;

		if(clippedX != pos.x || clippedY != pos.y)
		{
			pos.x = clippedX;
			pos.y = clippedY;

			return true;
		}

		return false;
	}

	void ClipCursorDisable(Platform::Pimpl* data)
	{
		data->CursorClipEnabled = false;
		data->CursorClipWindow = None;
	}

	void SetCurrentCursor(Platform::Pimpl* data, ::Cursor cursor)
	{
		if(data->CurrentCursor)
		{
			XFreeCursor(data->XDisplay, data->CurrentCursor);
		}

		data->CurrentCursor = cursor;
		ApplyCurrentCursor(data, data->MainXWindow);
	}

	Vector2I Platform::GetCursorPosition()
    {
		Vector2I pos;
		UINT32 screenCount = (UINT32)XScreenCount(_data->XDisplay);

		for (UINT32 i = 0; i < screenCount; ++i)
		{
			::Window outRoot, outChild;
			INT32 childX, childY;
			UINT32 mask;

			if(XQueryPointer(_data->XDisplay, XRootWindow(_data->XDisplay, i), 
				&outRoot, &outChild, &pos.x, &pos.y, &childX, &childY, &mask))
			{
				break;
			}
		}

		return pos;
	}

	void Platform::SetCursorPosition(const Vector2I& screenPos)
	{
		UINT32 screenCount = (UINT32)XScreenCount(_data->XDisplay);

		// Note assuming screens are laid out horizontally left to right
		INT32 screenX = 0;
		for(UINT32 i = 0; i < screenCount; ++i)
		{
			::Window root = XRootWindow(_data->XDisplay, i);
			INT32 screenXEnd = screenX + XDisplayWidth(_data->XDisplay, i);

			if(screenPos.x >= screenX && screenPos.x < screenXEnd)
			{
				XWarpPointer(_data->XDisplay, None, root, 0, 0, 0, 0, screenPos.x, screenPos.y);
				XFlush(_data->XDisplay);
				return;
			}

			screenX = screenXEnd;
		}
	}

	void Platform::CaptureMouse(const RenderWindow& window)
	{
		LinuxWindow* linuxWindow;
		window.GetCustomAttribute("LINUX_WINDOW", &linuxWindow);

		UINT32 mask = ButtonPressMask | ButtonReleaseMask | PointerMotionMask | FocusChangeMask;
		XGrabPointer(_data->XDisplay, linuxWindow->_getXWindow(), False, mask, GrabModeAsync,
				GrabModeAsync, None, None, CurrentTime);
		XSync(_data->XDisplay, False);
	}

	void Platform::ReleaseMouseCapture()
	{
		XUngrabPointer(_data->XDisplay, CurrentTime);
		XSync(_data->XDisplay, False);
	}

	bool Platform::IsPointOverWindow(const RenderWindow& window, const Vector2I& screenPos)
	{
		// Single window application, always return true
		return true;
	}

	void Platform::HideCursor()
	{
		_data->IsCursorHidden = true;
		ApplyCurrentCursor(_data, _data->MainXWindow);
	}

	void Platform::ShowCursor()
	{
		_data->IsCursorHidden = false;
		ApplyCurrentCursor(_data, _data->MainXWindow);
	}

	bool Platform::IsCursorHidden()
	{
		return _data->IsCursorHidden;
	}

	void Platform::ClipCursorToWindow(const RenderWindow& window)
	{
		LinuxWindow* linuxWindow;
		window.GetCustomAttribute("LINUX_WINDOW", &linuxWindow);

		_data->CursorClipEnabled = true;
		_data->CursorClipWindow = linuxWindow;

		UpdateClipBounds(_data, linuxWindow);

		Vector2I pos = GetCursorPosition();

		if(ClipCursor(_data, pos))
		{
			SetCursorPosition(pos);
		}
	}

	void Platform::ClipCursorToRect(const Rect2I& screenRect)
	{
		_data->CursorClipEnabled = true;
		_data->CursorClipRect = screenRect;
		_data->CursorClipWindow = nullptr;

		Vector2I pos = GetCursorPosition();

		if(ClipCursor(_data, pos))
		{
			SetCursorPosition(pos);
		}	
	}

	void Platform::ClipCursorDisable()
	{
		te::ClipCursorDisable(_data);
	}

	void Platform::Update()
	{
		MessagePump();
	}

	void Platform::MessagePump()
	{
		while(true)
		{
			if(XPending(_data->XDisplay) <= 0)
				break;

			XEvent event;
			XNextEvent(_data->XDisplay, &event);

			if (event.type == KeyPress)
            	break;
		}
	}

	void Platform::StartUp()
	{
		_data->XDisplay = XOpenDisplay(NULL);
		XSetErrorHandler(X11ErrorHandler);

		// For raw, relative mouse motion events, XInput2 extension is required
		/*
		int firstEvent;
		int firstError;
		if (!XQueryExtension(_data->XDisplay, "XInputExtension", &_data->XInput2Opcode, &firstEvent, &firstError))
			TE_ASSERT_ERROR(false, "X Server doesn't support the XInput extension", __FILE__, __LINE__);

		int majorVersion = 2;
		int minorVersion = 0;
		if (XIQueryVersion(_data->XDisplay, &majorVersion, &minorVersion) != Success)
			TE_ASSERT_ERROR(false, "X Server doesn't support at least the XInput 2.0 extension", __FILE__, __LINE__);
		*/
		// Let XInput know we are interested in raw mouse movement events
		/*
		constexpr int maskLen = XIMaskLen(XI_LASTEVENT);
		XIEventMask mask;
		mask.deviceid = XIAllDevices;
		mask.mask_len = maskLen;

		unsigned char maskBuffer[maskLen] = {0};
		mask.mask = maskBuffer;
		XISetMask(mask.mask, XI_RawMotion);
		*/
		// "RawEvents are sent exclusively to all root windows", so this should receive all events, even though we only
		// select on one display's root window (untested for lack of second screen).
		/*
		XISelectEvents(_data->XDisplay, XRootWindow(_data->XDisplay, DefaultScreen(_data->XDisplay)), &mask, 1);
		XFlush(_data->XDisplay);
		*/
	}

	::Display* LinuxPlatform::GetXDisplay()
	{
		return _data->XDisplay;
	}

	::Window LinuxPlatform::GetMainXWindow()
	{
		return _data->MainXWindow;
	}

	void LinuxPlatform::LockX()
	{
		// TODO
	}

	void LinuxPlatform::UnlockX()
	{
		// TODO
	}

	void LinuxPlatform::RegisterWindow(::Window xWindow, LinuxWindow* window)
	{
		// First window is assumed to be the main
		if(_data->MainXWindow == 0)
		{
			_data->MainXWindow = xWindow;

			// Input context client window must be set before use
			/*
			XSetICValues(_data->IC,
					XNClientWindow, xWindow,
					XNFocusWindow, xWindow,
					nullptr);
			*/
		}

		//_data->EmptyCursor = XCreateFontCursor(_data->XDisplay, XC_arrow); 
		//_data->CurrentCursor = XCreateFontCursor(_data->XDisplay, XC_arrow); 

		//ApplyCurrentCursor(_data, xWindow);
	}
}