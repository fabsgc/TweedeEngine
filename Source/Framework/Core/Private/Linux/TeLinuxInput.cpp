#include "Input/TeInput.h"
#include "Error/TeError.h"
#include "Private/Linux/TeLinuxInput.h"
#include "Input/TeMouse.h"
#include "Input/TeKeyboard.h"
#include "Input/TeGamePad.h"

namespace te
{
    void Input::InitRawInput()
    {
        // TODO
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

        te_delete(_platformData);
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