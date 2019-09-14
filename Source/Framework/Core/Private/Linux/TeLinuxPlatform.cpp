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
		// TODO
	}

	void UpdateClipBounds(Platform::Pimpl* data, LinuxWindow* window)
	{
		// TODO
	}

	bool ClipCursor(Platform::Pimpl* data, Vector2I& pos)
	{
		// TODO

		return true;
	}

	void ClipCursorDisable(Platform::Pimpl* data)
	{
		// TODO
	}

	void SetCurrentCursor(Platform::Pimpl* data, ::Cursor cursor)
	{
		// TODO
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
			SetCursorPosition(pos);
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

	}
}