#include "Input/TeGamePad.h"
#include "Input/TeInput.h"
#include "Private/Win32/TeWin32Input.h"

namespace te
{
    /** Contains state of a POV (DPad). */
    struct POVState
    {
        ButtonCode code;
        bool pressed;
    };

    /** Contains private data for the Win32 Gamepad implementation. */
    struct GamePad::Pimpl
    {
        IDirectInput8* DirectInput;
        IDirectInputDevice8* Gamepad;
        GamePadInfo Info;
        DWORD CoopSettings;
        HWND HWnd;

        POVState PovState[4];
        INT32 AxisState[6]; // Only for XInput
        bool ButtonState[16]; // Only for XInput
    };

    /**
     * Initializes DirectInput gamepad device for a window with the specified handle. Only input from that window will be
     * reported.
     */
    void InitializeDirectInput(GamePad::Pimpl* data, HWND hWnd)
    {
        DIPROPDWORD dipdw;
        dipdw.diph.dwSize = sizeof(DIPROPDWORD);
        dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        dipdw.diph.dwObj = 0;
        dipdw.diph.dwHow = DIPH_DEVICE;
        dipdw.dwData = DI_BUFFER_SIZE_GAMEPAD;

        if (FAILED(data->DirectInput->CreateDevice(data->Info.GuidInstance, &data->Gamepad, nullptr)))
            TE_ASSERT_ERROR(false, "DirectInput gamepad init: Failed to create device.", __FILE__, __LINE__);

        if (FAILED(data->Gamepad->SetDataFormat(&c_dfDIJoystick2)))
            TE_ASSERT_ERROR(false, "DirectInput gamepad init: Failed to set format.", __FILE__, __LINE__);

        if (FAILED(data->Gamepad->SetCooperativeLevel(hWnd, data->CoopSettings)))
            TE_ASSERT_ERROR(false, "DirectInput gamepad init: Failed to set coop level.", __FILE__, __LINE__);

        if (FAILED(data->Gamepad->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)))
            TE_ASSERT_ERROR(false, "DirectInput gamepad init: Failed to set property.", __FILE__, __LINE__);

        HRESULT hr = data->Gamepad->Acquire();
        if (FAILED(hr) && hr != DIERR_OTHERAPPHASPRIO)
            TE_ASSERT_ERROR(false, "DirectInput gamepad init: Failed to acquire device.", __FILE__, __LINE__);

        data->HWnd = hWnd;
    }

    /** Releases DirectInput resources for the provided device */
    void ReleaseDirectInput(GamePad::Pimpl* data)
    {
        if (data->Gamepad)
        {
            data->Gamepad->Unacquire();
            data->Gamepad->Release();
            data->Gamepad = nullptr;
        }
    }

    /** Handles a DirectInput POV event. */
    void HandlePOV(Input* owner, GamePad::Pimpl* data, int pov, DIDEVICEOBJECTDATA& di)
    {
        if (LOWORD(di.dwData) == 0xFFFF)
        {
            // Centered, release any buttons
            if (data->PovState[pov].pressed)
            {
                owner->NotifyButtonReleased(data->Info.Id, data->PovState[pov].code, di.dwTimeStamp);
                data->PovState[pov].pressed = false;
            }
        }
        else
        {
            POVState newPOVState;
            te_zero_out(newPOVState);

            switch (di.dwData)
            {
            case 0:
                newPOVState.code = BC_GAMEPAD_DPAD_UP;
                newPOVState.pressed = true;
                break;
            case 4500:
                newPOVState.code = BC_GAMEPAD_DPAD_UPRIGHT;
                newPOVState.pressed = true;
                break;
            case 9000:
                newPOVState.code = BC_GAMEPAD_DPAD_RIGHT;
                newPOVState.pressed = true;
                break;
            case 13500:
                newPOVState.code = BC_GAMEPAD_DPAD_DOWNRIGHT;
                newPOVState.pressed = true;
                break;
            case 18000:
                newPOVState.code = BC_GAMEPAD_DPAD_DOWN;
                newPOVState.pressed = true;
                break;
            case 22500:
                newPOVState.code = BC_GAMEPAD_DPAD_DOWNLEFT;
                newPOVState.pressed = true;
                break;
            case 27000:
                newPOVState.code = BC_GAMEPAD_DPAD_LEFT;
                newPOVState.pressed = true;
                break;
            case 31500:
                newPOVState.code = BC_GAMEPAD_DPAD_UPLEFT;
                newPOVState.pressed = true;
                break;
            }

            // Button was pressed
            if (newPOVState.pressed)
            {
                // Another button was previously pressed
                if (data->PovState[pov].pressed)
                {
                    // If its a different button, release the old one and press the new one
                    if (data->PovState[pov].code != newPOVState.code)
                    {
                        owner->NotifyButtonReleased(data->Info.Id, data->PovState[pov].code, di.dwTimeStamp);
                        owner->NotifyButtonPressed(data->Info.Id, newPOVState.code, di.dwTimeStamp);

                        data->PovState[pov].code = newPOVState.code;
                    }
                }
                else
                {
                    owner->NotifyButtonPressed(data->Info.Id, newPOVState.code, di.dwTimeStamp);
                    data->PovState[pov].code = newPOVState.code;
                    data->PovState[pov].pressed = true;
                }
            }
        }
    }

    /** Converts a DirectInput or XInput button code to Tweede ButtonCode. */
    ButtonCode GamepadButtonToButtonCode(INT32 code)
    {
        switch (code)
        {
        case 0:
            return BC_GAMEPAD_DPAD_UP;
        case 1:
            return BC_GAMEPAD_DPAD_DOWN;
        case 2:
            return BC_GAMEPAD_DPAD_LEFT;
        case 3:
            return BC_GAMEPAD_DPAD_RIGHT;
        case 4:
            return BC_GAMEPAD_START;
        case 5:
            return BC_GAMEPAD_BACK;
        case 6:
            return BC_GAMEPAD_LS;
        case 7:
            return BC_GAMEPAD_RS;
        case 8:
            return BC_GAMEPAD_LB;
        case 9:
            return BC_GAMEPAD_RB;
        case 10:
            return BC_GAMEPAD_BTN1;
        case 11:
            return BC_GAMEPAD_LS;
        case 12:
            return BC_GAMEPAD_A;
        case 13:
            return BC_GAMEPAD_B;
        case 14:
            return BC_GAMEPAD_X;
        case 15:
            return BC_GAMEPAD_Y;
        }

        return (ButtonCode)(BC_GAMEPAD_BTN1 + (code - 15));
    }

    GamePad::GamePad(const String& name, const GamePadInfo& gamepadInfo, Input* owner)
        : _name(name)
        , _owner(owner)
    {
        InputPrivateData* pvtData = owner->GetPrivateData();

        _data = te_new<Pimpl>();
        _data->DirectInput = pvtData->DirectInput;
        _data->CoopSettings = pvtData->MouseSettings;
        _data->Info = gamepadInfo;
        _data->Gamepad = nullptr;
        _data->HWnd = (HWND)owner->GetWindowHandle();
        te_zero_out(_data->PovState);
        te_zero_out(_data->AxisState);
        te_zero_out(_data->ButtonState);

        if (!_data->Info.IsXInput)
            InitializeDirectInput(_data, _data->HWnd);
    }

    GamePad::~GamePad()
    {
        ReleaseDirectInput(_data);
        te_delete(_data);
    }

    void GamePad::Capture()
    {
        if (_data->HWnd == (HWND)-1)
            return;

        if (_data->Info.IsXInput)
        {
            XINPUT_STATE inputState;
            if (XInputGetState((DWORD)_data->Info.XInputDev, &inputState) != ERROR_SUCCESS)
                memset(&inputState, 0, sizeof(inputState));

            // Sticks and triggers
            struct AxisState
            {
                bool Moved;
                INT32 Value;
            };

            AxisState axisState[6];
            te_zero_out(axisState);

            // Note: Order of axes must match InputAxis enum
            // Left stick
            axisState[0].Value = (int)inputState.Gamepad.sThumbLX;
            axisState[1].Value = -(int)inputState.Gamepad.sThumbLY;

            // Right stick 
            axisState[2].Value = (int)inputState.Gamepad.sThumbRX;
            axisState[3].Value = -(int)inputState.Gamepad.sThumbRY;

            // Left trigger
            axisState[4].Value = std::min((int)inputState.Gamepad.bLeftTrigger * 129, MAX_AXIS);

            // Right trigger
            axisState[5].Value = std::min((int)inputState.Gamepad.bRightTrigger * 129, MAX_AXIS);

            for (UINT32 i = 0; i < 6; i++)
            {
                axisState[i].Moved = axisState[i].Value != _data->AxisState[i];
                _data->AxisState[i] = axisState[i].Value;
            }

            // DPAD (POV)
            ButtonCode dpadButton = BC_UNASSIGNED;
            if ((inputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0)
                dpadButton = BC_GAMEPAD_DPAD_UP;
            else if ((inputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0)
                dpadButton = BC_GAMEPAD_DPAD_DOWN;
            if ((inputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0)
                dpadButton = BC_GAMEPAD_DPAD_LEFT;
            else if ((inputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0)
                dpadButton = BC_GAMEPAD_DPAD_RIGHT;

            if (dpadButton != BC_UNASSIGNED) // Pressed
            {
                // Another button was previously pressed
                if (_data->PovState[0].pressed)
                {
                    // If its a different button, release the old one and press the new one
                    if (_data->PovState[0].code != dpadButton)
                    {
                        _owner->NotifyButtonReleased(_data->Info.Id, _data->PovState[0].code, GetTickCount64());
                        _owner->NotifyButtonPressed(_data->Info.Id, dpadButton, GetTickCount64());

                        _data->PovState[0].code = dpadButton;
                    }
                }
                else
                {
                    _owner->NotifyButtonPressed(_data->Info.Id, dpadButton, GetTickCount64());
                    _data->PovState[0].code = dpadButton;
                    _data->PovState[0].pressed = true;
                }
            }
            else
            {
                if (_data->PovState[0].pressed)
                {
                    _owner->NotifyButtonReleased(_data->Info.Id, _data->PovState[0].code, GetTickCount64());
                    _data->PovState[0].pressed = false;
                }
            }

            // Buttons
            for (UINT32 i = 0; i < 16; i++)
            {
                bool buttonState = (inputState.Gamepad.wButtons & (1 << i)) != 0;

                if (buttonState != _data->ButtonState[i])
                {
                    if (buttonState)
                        _owner->NotifyButtonPressed(_data->Info.Id, GamepadButtonToButtonCode(i), GetTickCount64());
                    else
                        _owner->NotifyButtonReleased(_data->Info.Id, GamepadButtonToButtonCode(i), GetTickCount64());

                    _data->ButtonState[i] = buttonState;
                }
            }

            for (int i = 0; i < 6; ++i)
            {
                if (!axisState[i].Moved)
                    continue;

                _owner->NotifyAxisMoved(_data->Info.Id, i + (int)InputAxis::MouseZ, axisState[i].Value);
            }
        }
        else
        {
            if (_data->Gamepad == nullptr)
                return;

            DIDEVICEOBJECTDATA diBuff[DI_BUFFER_SIZE_GAMEPAD];
            DWORD numEntries = DI_BUFFER_SIZE_GAMEPAD;

            HRESULT hr = _data->Gamepad->Poll();
            if (hr == DI_OK)
                hr = _data->Gamepad->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), diBuff, &numEntries, 0);

            if (hr != DI_OK)
            {
                hr = _data->Gamepad->Acquire();
                while (hr == DIERR_INPUTLOST)
                    hr = _data->Gamepad->Acquire();

                _data->Gamepad->Poll();
                hr = _data->Gamepad->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), diBuff, &numEntries, 0);

                if (FAILED(hr))
                    return;
            }

            struct AxisState
            {
                bool moved;
                INT32 value;
            };

            AxisState axisState[24];
            te_zero_out(axisState);

            // Note: Not reporting slider or POV events
            for (UINT32 i = 0; i < numEntries; ++i)
            {
                switch (diBuff[i].dwOfs)
                {
                case DIJOFS_POV(0):
                    HandlePOV(_owner, _data, 0, diBuff[i]);
                    break;
                case DIJOFS_POV(1):
                    HandlePOV(_owner, _data, 1, diBuff[i]);
                    break;
                case DIJOFS_POV(2):
                    HandlePOV(_owner, _data, 2, diBuff[i]);
                    break;
                case DIJOFS_POV(3):
                    HandlePOV(_owner, _data, 3, diBuff[i]);
                    break;
                default:
                    // Button event
                    if (diBuff[i].dwOfs >= DIJOFS_BUTTON(0) && diBuff[i].dwOfs < DIJOFS_BUTTON(128))
                    {
                        int button = diBuff[i].dwOfs - DIJOFS_BUTTON(0);

                        if ((diBuff[i].dwData & 0x80) != 0)
                            _owner->NotifyButtonPressed(_data->Info.Id, GamepadButtonToButtonCode(button), diBuff[i].dwTimeStamp);
                        else
                            _owner->NotifyButtonReleased(_data->Info.Id, GamepadButtonToButtonCode(button), diBuff[i].dwTimeStamp);

                    }
                    
                    if ((short)(diBuff[i].uAppData >> 16) == 0x1313) // Axis event
                    {
                        int axis = (int)(0x0000FFFF & diBuff[i].uAppData);
                        if (axis < 24)
                        {
                            axisState[axis].moved = true;
                            axisState[axis].value = diBuff[i].dwData;
                        }
                    }
                }
            }

            if (numEntries > 0)
            {
                for (int i = 0; i < 24; ++i)
                {
                    if (!axisState[i].moved)
                        continue;

                    _owner->NotifyAxisMoved(_data->Info.Id, i + (int)InputAxis::MouseZ, axisState[i].value);
                }
            }
        }
    }

    void GamePad::ChangeCaptureContext(UINT64 windowHandle)
    {
        HWND newhWnd = (HWND)windowHandle;

        if (_data->HWnd != newhWnd)
        {
            ReleaseDirectInput(_data);

            if (windowHandle != (UINT64)-1)
                InitializeDirectInput(_data, newhWnd);
            else
                _data->HWnd = (HWND)-1;
        }
    }
}