#include "Input/TeInput.h"
#include "Error/TeError.h"
#include "Private/Linux/TeLinuxInput.h"
#include "Input/TeMouse.h"
#include "Input/TeKeyboard.h"
#include "Input/TeGamePad.h"
#include <fcntl.h>
#include <linux/input.h>

namespace te
{
    /** Information about events reported from a specific input event device. */
	struct EventInfo
	{
		Vector<INT32> Buttons;
		Vector<INT32> RelAxes;
		Vector<INT32> AbsAxes;
		Vector<INT32> Hats;
	};

    /** Checks is the bit at the specified location in a byte array is set. */
	bool IsBitSet(UINT8 bits[], UINT32 bit)
	{
		return ((bits[bit/8] >> (bit%8)) & 1) != 0;
	}

    /** Returns information about an input event device attached to he provided file handle. */
	bool GetEventInfo(int fileHandle, EventInfo& eventInfo)
	{
        UINT8 eventBits[1 + EV_MAX/8];
		te_zero_out(eventBits);

        if (ioctl(fileHandle, EVIOCGBIT(0, sizeof(eventBits)), eventBits) == -1)
			return false;

        for (UINT32 i = 0; i < EV_MAX; i++)
		{
			if(IsBitSet(eventBits, i))
			{
                if(i == EV_ABS)
				{
                    UINT8 absAxisBits[1 + ABS_MAX/8];
					te_zero_out(absAxisBits);

					if (ioctl(fileHandle, EVIOCGBIT(i, sizeof(absAxisBits)), absAxisBits) == -1)
					{
						TE_DEBUG("Could not read device absolute axis features.", __FILE__, __LINE__);
						continue;
					}

					for (UINT32 j = 0; j < ABS_MAX; j++)
					{
						if(IsBitSet(absAxisBits, j))
						{
							if(j >= ABS_HAT0X && j <= ABS_HAT3Y)
                            {
								eventInfo.Hats.push_back(j);
                            }
							else
                            {
								eventInfo.AbsAxes.push_back(j);
                            }
						}
					}
                }
                else if(i == EV_REL)
				{
                    UINT8 relAxisBits[1 + REL_MAX/8];
					te_zero_out(relAxisBits);

					if (ioctl(fileHandle, EVIOCGBIT(i, sizeof(relAxisBits)), relAxisBits) == -1)
					{
						TE_DEBUG("Could not read device relative axis features.", __FILE__, __LINE__);
						continue;
					}

					for (UINT32 j = 0; j < REL_MAX; j++)
					{
						if(IsBitSet(relAxisBits, j))
                        {
							eventInfo.RelAxes.push_back(j);
                        }
					}
                }
                else if(i == EV_KEY)
				{
                    UINT8 keyBits[1 + KEY_MAX/8];
					te_zero_out(keyBits);

					if (ioctl(fileHandle, EVIOCGBIT(i, sizeof(keyBits)), keyBits) == -1)
					{
						TE_DEBUG("Could not read device key features.", __FILE__, __LINE__);
						continue;
					}

					for (UINT32 j = 0; j < KEY_MAX; j++)
					{
						if(IsBitSet(keyBits, j))
                        {
							eventInfo.Buttons.push_back(j);
                        }
					}
                }
            }
        }

        return true;
    }

    /** Converts a Linux button code to Banshee ButtonCode. */
	ButtonCode GamepadMapCommonButton(INT32 code)
	{
		// Note: Assuming XBox controller layout here
		switch (code)
		{
            case BTN_TRIGGER_HAPPY1:
                return TE_GAMEPAD_DPAD_LEFT;
            case BTN_TRIGGER_HAPPY2:
                return TE_GAMEPAD_DPAD_RIGHT;
            case BTN_TRIGGER_HAPPY3:
                return TE_GAMEPAD_DPAD_UP;
            case BTN_TRIGGER_HAPPY4:
                return TE_GAMEPAD_DPAD_DOWN;
            case BTN_START:
                return TE_GAMEPAD_START;
            case BTN_SELECT:
                return TE_GAMEPAD_BACK;
            case BTN_THUMBL:
                return TE_GAMEPAD_LS;
            case BTN_THUMBR:
                return TE_GAMEPAD_RS;
            case BTN_TL:
                return TE_GAMEPAD_LB;
            case BTN_TR:
                return TE_GAMEPAD_RB;
            case BTN_A:
                return TE_GAMEPAD_A;
            case BTN_B:
                return TE_GAMEPAD_B;
            case BTN_X:
                return TE_GAMEPAD_X;
            case BTN_Y:
                return TE_GAMEPAD_Y;
		}

		return TE_UNASSIGNED;
	}

	/**
	 * Maps an absolute axis as reported by the Linux system, to a Banshee axis. This will be one of the InputAxis enum
	 * members, or -1 if it cannot be mapped.
	 */
	INT32 gamepadMapCommonAxis(INT32 axis)
	{
		switch(axis)
		{
            case ABS_X: return (INT32)InputAxis::LeftStickX;
            case ABS_Y: return (INT32)InputAxis::LeftStickY;
            case ABS_RX: return (INT32)InputAxis::RightStickX;
            case ABS_RY: return (INT32)InputAxis::RightStickY;
            case ABS_Z: return (INT32)InputAxis::LeftTrigger;
            case ABS_RZ: return (INT32)InputAxis::RightTrigger;
		}

		return -1;
	}

    /**
	 * Returns true if the input event attached to the specified file handle is a gamepad,
	 * and populates the gamepad info structure. Returns false otherwise.
	 */
	bool ParseGamePadInfo(int fileHandle, int eventHandlerIdx, GamePadInfo& info)
	{
        EventInfo eventInfo;
		if(!GetEventInfo(fileHandle, eventInfo))
			return false;

		bool isGamepad = false;

        // Check for gamepad buttons
		UINT32 unknownButtonIdx = 0;
		for(auto& entry : eventInfo.Buttons)
		{
			if((entry >= BTN_JOYSTICK && entry < BTN_GAMEPAD)
				|| (entry >= BTN_GAMEPAD && entry < BTN_DIGI)
				|| (entry >= BTN_WHEEL && entry < KEY_OK))
			{
				ButtonCode bc = GamepadMapCommonButton(entry);
				if(bc == TE_UNASSIGNED)
				{
					// Map to unnamed buttons
					if(unknownButtonIdx < 20)
					{
						bc = (ButtonCode)((INT32)TE_GAMEPAD_BTN1 + unknownButtonIdx);
						info.ButtonMap[entry] = bc;

						unknownButtonIdx++;
					}
				}
				else
					info.ButtonMap[entry] = bc;

				isGamepad = true;
			}
		}

        if(isGamepad)
		{
			info.EventHandlerIdx = eventHandlerIdx;

			// Get device name
			char name[128];
			if (ioctl(fileHandle, EVIOCGNAME(sizeof(name)), name) != -1)
				info.Name = String(name);
			else
				TE_DEBUG("Could not read device name.", __FILE__, __LINE__);

			// Get axis ranges
			UINT32 unknownAxisIdx = 0;
			for(auto& entry : eventInfo.AbsAxes)
			{
				AxisInfo& axisInfo = info.AxisMap[entry];
				axisInfo.Min = GamePad::MIN_AXIS;
				axisInfo.Max = GamePad::MAX_AXIS;

				input_absinfo absinfo;
				if (ioctl(fileHandle, EVIOCGABS(entry), &absinfo) == -1)
				{
					TE_DEBUG("Could not read absolute axis device features.", __FILE__, __LINE__);
					continue;
				}

				axisInfo.Min = absinfo.minimum;
				axisInfo.Max = absinfo.maximum;

				axisInfo.AxisIdx = gamepadMapCommonAxis(entry);
				if(axisInfo.AxisIdx == -1)
				{
					axisInfo.AxisIdx = (INT32)InputAxis::Count + unknownAxisIdx;
					unknownAxisIdx++;
				}
			}
		}

        return isGamepad;
    }

    void Input::InitRawInput()
    {
        _platformData = te_new<InputPrivateData>();

		// Scan for valid gamepad devices
		for(int i = 0; i < 64; ++i )
		{
			String eventPath = "/dev/input/event" + ToString(i);
			int file = open(eventPath.c_str(), O_RDONLY |O_NONBLOCK);
			if(file == -1)
			{
				// Note: We're ignoring failures due to permissions. The assumption is that gamepads won't have special
				// permissions. If this assumption proves wrong, then using udev might be required to read gamepad input.
				continue;
			}

			GamePadInfo info;
			if(ParseGamePadInfo(file, i, info))
			{
				info.Id = (UINT32)_platformData->GamepadInfos.size();
				_platformData->GamepadInfos.push_back(info);
			}
            else
            {
                TE_DEBUG("Can't parse GamePadInfo", __FILE__, __LINE__);
            }
            
			close(file);
		}

		_keyboard = te_new<Keyboard>("Keyboard", this);
		_mouse = te_new<Mouse>("Mouse", this);

		UINT32 numGamepads = GetDeviceCount(InputDevice::Gamepad);
		for (UINT32 i = 0; i < numGamepads; i++)
        {
			_gamepads.push_back(te_new<GamePad>(_platformData->GamepadInfos[i].Name, _platformData->GamepadInfos[i], this));
        }
    }

    void Input::CleanUpRawInput()
    {
        if (_mouse != nullptr)
        {
            te_delete(_mouse);
        }

        if (_keyboard != nullptr)
        {
            te_delete(_keyboard);
        }

        for (auto& gamepad : _gamepads)
        {
            te_delete(gamepad);
        }

        if(_platformData != nullptr)
        {
            te_delete(_platformData);
        }
    }

    UINT32 Input::GetDeviceCount(InputDevice device) const
    {
        switch (device)
        {
            case InputDevice::Keyboard: return 1;
            case InputDevice::Mouse: return 1;
            case InputDevice::Gamepad: return (UINT32)_platformData->GamepadInfos.size();
            default:
            case InputDevice::Count: return 0;
        }
    }
}