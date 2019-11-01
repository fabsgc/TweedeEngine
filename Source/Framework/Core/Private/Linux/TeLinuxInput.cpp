#include "Input/TeInput.h"
#include "Error/TeError.h"
#include "Private/Linux/TeLinuxInput.h"
#include "Input/TeMouse.h"
#include "Input/TeKeyboard.h"
#include "Input/TeGamePad.h"

#include <fcntl.h>

namespace te
{
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

			/*GamepadInfo info;
			if(parseGamepadInfo(file, i, info))
			{
				info.Id = (UINT32)_platformData->GamepadInfos.size();
				_platformData->GamepadInfos.push_back(info);
			}*/

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