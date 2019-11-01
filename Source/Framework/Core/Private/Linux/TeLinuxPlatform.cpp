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

		Atom AtomDeleteWindow;
		Atom AtomWmState;
		Atom AtomWmStateHidden;
		Atom AtomWmStateMaxVert;
		Atom AtomWmStateMaxHorz;

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

		LinuxWindow* Window;
    };

	int X11ErrorHandler(::Display* display, XErrorEvent* event)
	{
		// X11 by default crashes the app on error, even though some errors can be just fine. So we provide our own handler.

		char buffer[256];
		XGetErrorText(display, event->error_code, buffer, sizeof(buffer));
		TE_DEBUG("X11 error: " + String(buffer), __FILE__, __LINE__);

		return 0;
	}

	/** Maps X11 mouse button codes to Banshee button codes. */
	ButtonCode XButtonToButtonCode(int button)
	{
		switch (button)
		{
		case Button1:
			return TE_MOUSE_LEFT;
		case Button2:
			return TE_MOUSE_MIDDLE;
		case Button3:
			return TE_MOUSE_RIGHT;
		default:
			return (ButtonCode)(TE_MOUSE_LEFT + button - 1);
		}
	}

	/** Maps engine button codes to X11 names for physical key locations. */
	const char* ButtonCodeToKeyName(ButtonCode code)
	{
		switch(code)
		{
			// Row #1
		case TE_ESCAPE:		return "ESC";
		case TE_F1:			return "FK01";
		case TE_F2:			return "FK02";
		case TE_F3:			return "FK03";
		case TE_F4:			return "FK04";
		case TE_F5:			return "FK05";
		case TE_F6:			return "FK06";
		case TE_F7:			return "FK07";
		case TE_F8:			return "FK08";
		case TE_F9:			return "FK09";
		case TE_F10:		return "FK10";
		case TE_F11:		return "FK11";
		case TE_F12:		return "FK12";
		case TE_F13:		return "FK13";
		case TE_F14:		return "FK14";
		case TE_F15:		return "FK15";

			// Row #2
		case TE_GRAVE:		return "TLDE";
		case TE_1: 			return "AE01";
		case TE_2:			return "AE02";
		case TE_3:			return "AE03";
		case TE_4:			return "AE04";
		case TE_5:			return "AE05";
		case TE_6:			return "AE06";
		case TE_7:			return "AE07";
		case TE_8:			return "AE08";
		case TE_9:			return "AE09";
		case TE_0:			return "AE10";
		case TE_MINUS:		return "AE11";
		case TE_EQUALS:		return "AE12";
		case TE_BACK:		return "BKSP";

			// Row #3
		case TE_TAB:		return "TAB";
		case TE_Q:			return "AD01";
		case TE_W:			return "AD02";
		case TE_E:			return "AD03";
		case TE_R:			return "AD04";
		case TE_T:			return "AD05";
		case TE_Y:			return "AD06";
		case TE_U:			return "AD07";
		case TE_I:			return "AD08";
		case TE_O:			return "AD09";
		case TE_P:			return "AD10";
		case TE_LBRACKET:	return "AD11";
		case TE_RBRACKET:	return "AD12";
		case TE_RETURN:		return "RTRN";

			// Row #4
		case TE_CAPITAL:	return "CAPS";
		case TE_A:			return "AC01";
		case TE_S:			return "AC02";
		case TE_D:			return "AC03";
		case TE_F:			return "AC04";
		case TE_G:			return "AC05";
		case TE_H:			return "AC06";
		case TE_J:			return "AC07";
		case TE_K:			return "AC08";
		case TE_L:			return "AC09";
		case TE_SEMICOLON:	return "AC10";
		case TE_APOSTROPHE:	return "AC11";
		case TE_BACKSLASH:	return "BKSL";

			// Row #5
		case TE_LSHIFT:		return "LFSH";
		case TE_Z:			return "AB01";
		case TE_X:			return "AB02";
		case TE_C:			return "AB03";
		case TE_V:			return "AB04";
		case TE_B:			return "AB05";
		case TE_N:			return "AB06";
		case TE_M:			return "AB07";
		case TE_COMMA:		return "AB08";
		case TE_PERIOD:		return "AB09";
		case TE_SLASH:		return "AB10";
		case TE_RSHIFT:		return "RTSH";

			// Row #6
		case TE_LCONTROL:	return "LCTL";
		case TE_LWIN:		return "LWIN";
		case TE_LMENU:		return "LALT";
		case TE_SPACE:		return "SPCE";
		case TE_RMENU:		return "RALT";
		case TE_RWIN:		return "RWIN";
		case TE_RCONTROL:	return "RCTL";

			// Keypad
		case TE_NUMPAD0:	return "KP0";
		case TE_NUMPAD1:	return "KP1";
		case TE_NUMPAD2:	return "KP2";
		case TE_NUMPAD3:	return "KP3";
		case TE_NUMPAD4:	return "KP4";
		case TE_NUMPAD5:	return "KP5";
		case TE_NUMPAD6:	return "KP6";
		case TE_NUMPAD7:	return "KP7";
		case TE_NUMPAD8:	return "KP8";
		case TE_NUMPAD9:	return "KP9";

		case TE_NUMLOCK:		return "NMLK";
		case TE_DIVIDE:			return "KPDV";
		case TE_MULTIPLY:		return "KPMU";
		case TE_SUBTRACT:		return "KPSU";
		case TE_ADD:			return "KPAD";
		case TE_DECIMAL:		return "KPDL";
		case TE_NUMPADENTER:	return "KPEN";
		case TE_NUMPADEQUALS:	return "KPEQ";

			// Special keys
		case TE_SCROLL:		return "SCLK";
		case TE_PAUSE:		return "PAUS";

		case TE_INSERT:		return "INS";
		case TE_HOME:		return "HOME";
		case TE_PGUP:		return "PGUP";
		case TE_DELETE:		return "DELE";
		case TE_END:		return "END";
		case TE_PGDOWN:		return "PGDN";

		case TE_UP:			return "UP";
		case TE_LEFT:		return "LEFT";
		case TE_DOWN:		return "DOWN";
		case TE_RIGHT:		return "RGHT";

		case TE_MUTE:		return "MUTE";
		case TE_VOLUMEDOWN:	return "VOL-";
		case TE_VOLUMEUP:	return "VOL+";
		case TE_POWER:		return "POWR";

			// International keys
		case TE_OEM_102:	return "LSGT"; // German keyboard: < > |
		case TE_KANA:		return "AB11"; // Taking a guess here, many layouts map <AB11> to "kana_RO"
		case TE_YEN:		return "AE13"; // Taking a guess, often mapped to yen

		default:
			// Missing Japanese (?): KATA, HIRA, HENK, MUHE, JPCM
			// Missing Korean (?): HNGL, HJCV
			// Missing because it's not clear which TE_ is correct: PRSC (print screen), LVL3 (AltGr), MENU
			// Misc: LNFD (line feed), I120, I126, I128, I129, COMP, STOP, AGAI (redo), PROP, UNDO, FRNT, COPY, OPEN, PAST
			// FIND, CUT, HELP, I147-I190, FK16-FK24, MDSW (mode switch), ALT, META, SUPR, HYPR, I208-I253
			break;
		}

		return nullptr;
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

	/**
	 * Enqueue a button press/release event to be handled by the main thread
	 * @param bc        ButtonCode for the button that was pressed or released
	 * @param pressed   true if the button was pressed, false if it was released
	 * @param timestamp Time when the event happened
	 */
	void EnqueueButtonEvent(ButtonCode bc, bool pressed, UINT64 timestamp)
	{
		if (bc == TE_UNASSIGNED)
			return;

		LinuxButtonEvent event;
		event.Button = bc;
		event.Pressed = pressed;
		event.Timestamp = timestamp;
		LinuxPlatform::ButtonEvents.push(event);
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
		XGrabPointer(_data->XDisplay, linuxWindow->GetXWindow(), False, mask, GrabModeAsync,
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

			XGenericEventCookie* cookie = &event.xcookie;
			if (cookie->type == GenericEvent && cookie->extension == _data->XInput2Opcode)
			{
				XGetEventData(_data->XDisplay, cookie);
				XIRawEvent* xInput2Event = (XIRawEvent*) cookie->data;
				switch (xInput2Event->evtype)
				{
					case XI_RawMotion:
					{
						if (xInput2Event->valuators.mask_len > 0)
						{
							// Assume X/Y delta is stored in valuators 0/1 and vertical scroll in valuator 3.
							// While there is an API that reliably tells us the valuator index for vertical scroll, there's
							// nothing "more reliable" for X/Y axes, as the only way to possibly identify them from device
							// info is by axis name, so we can use the axis index directly just as well. GDK seems to assume
							// 0 for x and 1 for y too, so that's hopefully safe, and 3 appears to be common for the scroll
							// wheel.
							float deltas[4] = {0};
							int currentValuesIndex = 0;

							for (UINT8 valuator = 0; valuator < 4; valuator++)
							{
								if (XIMaskIsSet(xInput2Event->valuators.mask, valuator))
								{
									deltas[valuator] = xInput2Event->raw_values[currentValuesIndex++];
								}
							}

							LinuxPlatform::MouseMotionEvent.DeltaX += deltas[0];
							LinuxPlatform::MouseMotionEvent.DeltaY += deltas[1];
							LinuxPlatform::MouseMotionEvent.DeltaZ += deltas[3];
						}
					}
					break;
				}

				XFreeEventData(_data->XDisplay, cookie);
			}

			switch (event.type)
			{
				case ClientMessage:
				{
					// User requested the window to close
					if((Atom)event.xclient.data.l[0] == _data->AtomDeleteWindow)
					{
						RenderWindow* renderWindow = _data->Window->GetRenderWindow();
						if(renderWindow != nullptr)
						{
							renderWindow->NotifyWindowEvent(WindowEventType::CloseRequested);
						}
					}
				}
				break;

				case KeyPress:
				{
					XKeyPressedEvent* keyEvent = (XKeyPressedEvent*) &event;
					EnqueueButtonEvent(_data->KeyCodeMap[keyEvent->keycode], true, (UINT64) keyEvent->time);
				}
				break;

				case KeyRelease:
				{
					XKeyReleasedEvent* keyEvent = (XKeyReleasedEvent*) &event;
					EnqueueButtonEvent(_data->KeyCodeMap[keyEvent->keycode], false, (UINT64) keyEvent->time);
				}
				break;

				case ButtonPress:
				{
					XButtonPressedEvent* buttonEvent = (XButtonPressedEvent*) &event;
					UINT32 button = event.xbutton.button;

					EnqueueButtonEvent(XButtonToButtonCode(button), true, (UINT64) buttonEvent->time);

					OSPointerButtonStates btnStates;
					btnStates.MouseButtons[0] = (event.xbutton.state & Button1Mask) != 0;
					btnStates.MouseButtons[1] = (event.xbutton.state & Button2Mask) != 0;
					btnStates.MouseButtons[2] = (event.xbutton.state & Button3Mask) != 0;

					OSMouseButton mouseButton;
					bool validPress = false;
					switch(button)
					{
						case Button1:
							mouseButton = OSMouseButton::Left;
							btnStates.MouseButtons[0] = true;
							validPress = true;
						break;
						
						case Button2:
							mouseButton = OSMouseButton::Middle;
							btnStates.MouseButtons[1] = true;
							validPress = true;
						break;
						
						case Button3:
							mouseButton = OSMouseButton::Right;
							btnStates.MouseButtons[2] = true;
							validPress = true;
						break;

						default:
						break;
					}

					if(validPress)
					{
						// Send event
						Vector2I pos;
						pos.x = event.xbutton.x_root;
						pos.y = event.xbutton.y_root;

						btnStates.Ctrl = (event.xbutton.state & ControlMask) != 0;
						btnStates.Shift = (event.xbutton.state & ShiftMask) != 0;

						OnCursorButtonPressed(pos, mouseButton, btnStates);

						// Handle double-click
						if(button == Button1)
						{
							if (event.xbutton.time < (_data->LastButtonPressTime + DOUBLE_CLICK_MS))
							{
								OnCursorDoubleClick(pos, btnStates);
								_data->LastButtonPressTime = 0;
							}
							else
							{
								_data->LastButtonPressTime = event.xbutton.time;
							}
						}
					}
				}
				break;

				case ButtonRelease:
				{
					XButtonReleasedEvent* buttonEvent = (XButtonReleasedEvent*) &event;
					UINT32 button = event.xbutton.button;
					EnqueueButtonEvent(XButtonToButtonCode(button), false, (UINT64) buttonEvent->time);

					Vector2I pos;
					pos.x = event.xbutton.x_root;
					pos.y = event.xbutton.y_root;

					OSPointerButtonStates btnStates;
					btnStates.Ctrl = (event.xbutton.state & ControlMask) != 0;
					btnStates.Shift = (event.xbutton.state & ShiftMask) != 0;
					btnStates.MouseButtons[0] = (event.xbutton.state & Button1Mask) != 0;
					btnStates.MouseButtons[1] = (event.xbutton.state & Button2Mask) != 0;
					btnStates.MouseButtons[2] = (event.xbutton.state & Button3Mask) != 0;

					switch(button)
					{
						case Button1:
							btnStates.MouseButtons[0] = false;
							OnCursorButtonReleased(pos, OSMouseButton::Left, btnStates);
						break;
						case Button2:
							btnStates.MouseButtons[1] = false;
							OnCursorButtonReleased(pos, OSMouseButton::Middle, btnStates);
						break;
						case Button3:
							btnStates.MouseButtons[2] = false;
							OnCursorButtonReleased(pos, OSMouseButton::Right, btnStates);
						break;
						case Button4: // Vertical mouse wheel
						case Button5:
						{
							INT32 delta = button == Button4 ? 1 : -1;
							OnMouseWheelScrolled((float)delta);
						}
						break;
						default:
						break;
					}
				}
				break;

				case MotionNotify:
				{
					Vector2I pos;
					pos.x = event.xmotion.x_root;
					pos.y = event.xmotion.y_root;

					// Handle clipping if enabled
					if(ClipCursor(_data, pos))
					{
						SetCursorPosition(pos);
					}

					// Send event
					OSPointerButtonStates btnStates;
					btnStates.Ctrl = (event.xmotion.state & ControlMask) != 0;
					btnStates.Shift = (event.xmotion.state & ShiftMask) != 0;
					btnStates.MouseButtons[0] = (event.xmotion.state & Button1Mask) != 0;
					btnStates.MouseButtons[1] = (event.xmotion.state & Button2Mask) != 0;
					btnStates.MouseButtons[2] = (event.xmotion.state & Button3Mask) != 0;

					OnCursorMoved(pos, btnStates);
				}
				break;

				case EnterNotify:
					// Nothing to do
				break;

				case LeaveNotify:
				{
					if (event.xcrossing.mode == NotifyNormal)
					{
						Vector2I pos;
						pos.x = event.xcrossing.x_root;
						pos.y = event.xcrossing.y_root;

						if (ClipCursor(_data, pos))
						{
							SetCursorPosition(pos);
						}
					}

					_data->Window->GetRenderWindow()->NotifyWindowEvent(WindowEventType::MouseLeft);
				}
				break;

				case ConfigureNotify:
				{
					if(_data->Window != nullptr)
					{
						UpdateClipBounds(_data, _data->Window);

						RenderWindow* renderWindow = _data->Window->GetRenderWindow();
						if(renderWindow != nullptr)
						{
							renderWindow->NotifyWindowEvent(WindowEventType::Resized);
							renderWindow->NotifyWindowEvent(WindowEventType::Moved);
						}
					}
				}
				break;

				case FocusIn:
				{
					// Update input context focus
					XSetICFocus(_data->IC);

					// Send event to render window
					RenderWindow* renderWindow = _data->Window->GetRenderWindow();

					// Not a render window, so it doesn't care about these events
					if (renderWindow != nullptr)
					{
						if (!renderWindow->GetRenderWindowProperties().HasFocus)
						{
							renderWindow->NotifyWindowEvent(WindowEventType::FocusReceived);
						}
					}
				}
				break;

				case FocusOut:
				{
					// Update input context focus
					XUnsetICFocus(_data->IC);

					// Send event to render window
					RenderWindow* renderWindow = _data->Window->GetRenderWindow();

					// Not a render window, so it doesn't care about these events
					if (renderWindow != nullptr)
					{
						if (renderWindow->GetRenderWindowProperties().HasFocus)
							renderWindow->NotifyWindowEvent(WindowEventType::FocusLost);
					}
				}
				break;

				case PropertyNotify:
				break;

				default:
				break;
			}
		}
	}

	void Platform::StartUp()
	{
		_data->XDisplay = XOpenDisplay(NULL);
		XSetErrorHandler(X11ErrorHandler);

		// For raw, relative mouse motion events, XInput2 extension is required
		int firstEvent;
		int firstError;
		if (!XQueryExtension(_data->XDisplay, "XInputExtension", &_data->XInput2Opcode, &firstEvent, &firstError))
		{
			TE_ASSERT_ERROR(false, "X Server doesn't support the XInput extension", __FILE__, __LINE__);
		}

		int majorVersion = 2;
		int minorVersion = 0;
		if (XIQueryVersion(_data->XDisplay, &majorVersion, &minorVersion) != Success)
		{
			TE_ASSERT_ERROR(false, "X Server doesn't support at least the XInput 2.0 extension", __FILE__, __LINE__);
		}

		// Let XInput know we are interested in raw mouse movement events
		constexpr int maskLen = XIMaskLen(XI_LASTEVENT);
		XIEventMask mask;
		mask.deviceid = XIAllDevices;
		mask.mask_len = maskLen;

		unsigned char maskBuffer[maskLen] = {0};
		mask.mask = maskBuffer;
		XISetMask(mask.mask, XI_RawMotion);

		// "RawEvents are sent exclusively to all root windows", so this should receive all events, even though we only
		// select on one display's root window (untested for lack of second screen).
		XISelectEvents(_data->XDisplay, XRootWindow(_data->XDisplay, DefaultScreen(_data->XDisplay)), &mask, 1);
		XFlush(_data->XDisplay);

		if(XSupportsLocale())
		{
			XSetLocaleModifiers("@im=none");
			_data->IM = XOpenIM(_data->XDisplay, nullptr, nullptr, nullptr);

			// Note: Currently our windows don't support pre-edit and status areas, which are used for more complex types
			// of character input. Later on it might be beneficial to support them.
			_data->IC = XCreateIC(_data->IM, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, nullptr);
		}

		_data->AtomDeleteWindow = XInternAtom(_data->XDisplay, "WM_DELETE_WINDOW", False);
		_data->AtomWmState = XInternAtom(_data->XDisplay, "_NET_WM_STATE", False);
		_data->AtomWmStateHidden = XInternAtom(_data->XDisplay, "_NET_WM_STATE_HIDDEN", False);
		_data->AtomWmStateMaxHorz = XInternAtom(_data->XDisplay, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
		_data->AtomWmStateMaxVert = XInternAtom(_data->XDisplay, "_NET_WM_STATE_MAXIMIZED_VERT", False);

		// Initialize "unique X11 keyname" -> "X11 keycode" map
		char name[XkbKeyNameLength + 1];

		XkbDescPtr desc = XkbGetMap(_data->XDisplay, 0, XkbUseCoreKbd);
		XkbGetNames(_data->XDisplay, XkbKeyNamesMask, desc);

		for (UINT32 keyCode = desc->min_key_code; keyCode <= desc->max_key_code; keyCode++)
		{
			memcpy(name, desc->names->keys[keyCode].name, XkbKeyNameLength);
			name[XkbKeyNameLength] = '\0';

			_data->KeyNameMap[String(name)] = keyCode;
		}

		// Initialize "X11 keycode" -> "ButtonCode" map, based on the keyNameMap and keyCodeToKeyName()
		_data->KeyCodeMap.resize(desc->max_key_code + 1, TE_UNASSIGNED);

		XkbFreeNames(desc, XkbKeyNamesMask, True);
		XkbFreeKeyboard(desc, 0, True);

		for (UINT32 buttonCodeNum = TE_UNASSIGNED; buttonCodeNum <= TE_NumKeys; buttonCodeNum++)
		{
			ButtonCode buttonCode = (ButtonCode) buttonCodeNum;
			const char* keyNameCStr = ButtonCodeToKeyName(buttonCode);

			if (keyNameCStr != nullptr)
			{
				String keyName = String(keyNameCStr);
				auto iterFind = _data->KeyNameMap.find(keyName);
				if (iterFind != _data->KeyNameMap.end())
				{
					KeyCode keyCode = iterFind->second;
					_data->KeyCodeMap[keyCode] = buttonCode;
				}
			}
		}
	}

	void Platform::ShutDown()
	{
		TE_PRINT("closing");

		XFreeCursor(_data->XDisplay, _data->EmptyCursor);
		_data->EmptyCursor = None;

		if(_data->IC)
		{
			XDestroyIC(_data->IC);
			_data->IC = 0;
		}

		if(_data->IM)
		{
			XCloseIM(_data->IM);
			_data->IM = 0;
		}

		XCloseDisplay(_data->XDisplay);
		_data->XDisplay = nullptr;

		te_delete(_data);
		_data = nullptr;
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
		// NOTHING TO TODO
	}

	void LinuxPlatform::UnlockX()
	{
		// NOTHING TO TODO
	}

	void LinuxPlatform::RegisterWindow(::Window xWindow, LinuxWindow* window)
	{
		// First window is assumed to be the main
		if(_data->MainXWindow == 0)
		{
			_data->MainXWindow = xWindow;
			_data->Window = window;

			// Input context client window must be set before use
			XSetICValues(_data->IC,
					XNClientWindow, xWindow,
					XNFocusWindow, xWindow,
					nullptr);
		}

		_data->EmptyCursor = XCreateFontCursor(_data->XDisplay, XC_arrow); 
		_data->CurrentCursor = XCreateFontCursor(_data->XDisplay, XC_arrow); 

		ApplyCurrentCursor(_data, xWindow);
	}
}