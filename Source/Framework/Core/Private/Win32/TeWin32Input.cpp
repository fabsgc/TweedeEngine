#include "Input/TeInput.h"
#include "Error/TeError.h"
#include "Input/TeMouse.h"
#include "Input/TeGamePad.h"
#include "Input/TeKeyboard.h"
#include "Private/Win32/TeWin32Input.h"

namespace te
{
    BOOL CALLBACK _DIEnumDevCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
    {
        InputPrivateData* data = (InputPrivateData*)(pvRef);

        if (GET_DIDEVICE_TYPE(lpddi->dwDevType) == DI8DEVTYPE_JOYSTICK ||
            GET_DIDEVICE_TYPE(lpddi->dwDevType) == DI8DEVTYPE_GAMEPAD ||
            GET_DIDEVICE_TYPE(lpddi->dwDevType) == DI8DEVTYPE_1STPERSON ||
            GET_DIDEVICE_TYPE(lpddi->dwDevType) == DI8DEVTYPE_DRIVING ||
            GET_DIDEVICE_TYPE(lpddi->dwDevType) == DI8DEVTYPE_FLIGHT)
        {
            GamePadInfo gamepadInfo;
            gamepadInfo.Name = lpddi->tszInstanceName;
            gamepadInfo.GuidInstance = lpddi->guidInstance;
            gamepadInfo.GuidProduct = lpddi->guidProduct;
            gamepadInfo.Id = (UINT32)data->GamepadInfos.size();
            gamepadInfo.IsXInput = false;
            gamepadInfo.XInputDev = 0;

            data->GamepadInfos.push_back(gamepadInfo);
        }

        return DIENUM_CONTINUE;
    }

    void CheckXInputDevices(Vector<GamePadInfo>& infos)
    {
        bool cleanup = false;

        if (infos.size() == 0)
            return;

        HRESULT hr = CoInitialize(nullptr);
        bool cleanupCOM = SUCCEEDED(hr);

        BSTR classNameSpace = SysAllocString(L"\\\\.\\root\\cimv2");
        BSTR className = SysAllocString(L"Win32_PNPEntity");
        BSTR deviceID = SysAllocString(L"DeviceID");

        IWbemServices* IWbemServices = nullptr;
        IEnumWbemClassObject* enumDevices = nullptr;
        IWbemClassObject* devices[20] = { 0 };

        // Create WMI
        IWbemLocator* IWbemLocator = nullptr;
        hr = CoCreateInstance(__uuidof(WbemLocator), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IWbemLocator), (LPVOID*)&IWbemLocator);
        if (FAILED(hr) || IWbemLocator == nullptr)
            cleanup = true;

        if (classNameSpace == nullptr)
            cleanup = true;

        if (className == nullptr)
            cleanup = true;

        if (deviceID == nullptr)
            cleanup = true;

        if (IWbemLocator != nullptr)
        {
            // Connect to WMI
            hr = IWbemLocator->ConnectServer(classNameSpace, nullptr, nullptr, 0L, 0L, nullptr, nullptr, &IWbemServices);
            if (FAILED(hr) || IWbemServices == nullptr)
                cleanup = true;

            if (IWbemServices != nullptr)
            {
                // Switch security level to IMPERSONATE
                CoSetProxyBlanket(IWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);

                hr = IWbemServices->CreateInstanceEnum(className, 0, nullptr, &enumDevices);
                if (FAILED(hr) || enumDevices == nullptr)
                    cleanup = true;

                if (enumDevices != nullptr)
                {
                    // Loop over all devices
                    for (;; )
                    {
                        DWORD numDevices = 0;
                        hr = enumDevices->Next(5000, 20, devices, &numDevices);
                        if (FAILED(hr))
                            cleanup = true;

                        if (numDevices == 0)
                            break;

                        for (DWORD i = 0; i < numDevices; i++)
                        {
                            // For each device, get its device ID
                            VARIANT var;
                            hr = devices[i]->Get(deviceID, 0L, &var, nullptr, nullptr);
                            if (SUCCEEDED(hr) && var.vt == VT_BSTR && var.bstrVal != nullptr)
                            {
                                // Check if the device ID contains "IG_".  If it does, then it's an XInput device
                                if (wcsstr(var.bstrVal, L"IG_"))
                                {
                                    // If it does, then get the VID/PID from var.bstrVal
                                    DWORD dwPid = 0, dwVid = 0;
                                    WCHAR* strVid = wcsstr(var.bstrVal, L"VID_");
                                    if (strVid && swscanf_s(strVid, L"VID_%4X", &dwVid) != 1)
                                        dwVid = 0;

                                    WCHAR* strPid = wcsstr(var.bstrVal, L"PID_");
                                    if (strPid && swscanf_s(strPid, L"PID_%4X", &dwPid) != 1)
                                        dwPid = 0;

                                    // Compare the VID/PID to the DInput device
                                    DWORD dwVidPid = MAKELONG(dwVid, dwPid);

                                    for (auto entry = infos.begin(); entry != infos.end(); entry++)
                                    {
                                        if (dwVidPid == entry->GuidProduct.Data1)
                                        {
                                            entry->IsXInput = true;
                                            entry->XInputDev = (int)entry->Id; // Note: These might not match and I might need to get the XInput id differently
                                        }
                                    }
                                }
                            }

                            devices[i]->Release();
                        }
                    }
                }
            }
        }

        if (cleanup)
        {
            if (classNameSpace)
                SysFreeString(classNameSpace);

            if (deviceID)
                SysFreeString(deviceID);

            if (className)
                SysFreeString(className);

            for (DWORD i = 0; i < 20; i++)
                devices[i]->Release();

            enumDevices->Release();
            IWbemLocator->Release();
            IWbemServices->Release();

            if (cleanupCOM)
                CoUninitialize();
        }
    }

    void Input::InitRawInput()
    {
        _platformData = te_new<InputPrivateData>();

        if(IsWindow((HWND)_windowHandle) == 0)
            TE_ASSERT_ERROR(false, "RawInputManager failed to initialized. Invalid HWND provided.");

        HINSTANCE hInst = GetModuleHandle(0);

        HRESULT hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&_platformData->DirectInput, nullptr);
        if (FAILED(hr))
            TE_ASSERT_ERROR(false, "Unable to initialize DirectInput.");

        _platformData->KbSettings = DISCL_FOREGROUND | DISCL_NONEXCLUSIVE;
        _platformData->MouseSettings = DISCL_FOREGROUND | DISCL_NONEXCLUSIVE;

        // Enumerate all attached devices
        // Note: Only enumerating gamepads, assuming there is 1 keyboard and 1 mouse
        _platformData->DirectInput->EnumDevices((DWORD)NULL, _DIEnumDevCallback, _platformData, DIEDFL_ATTACHEDONLY);

        for (UINT32 i = 0; i < 4; ++i)
        {
            XINPUT_STATE state;
            if (XInputGetState(i, &state) != ERROR_DEVICE_NOT_CONNECTED)
            {
                CheckXInputDevices(_platformData->GamepadInfos);
                break;
            }
        }

        if (GetDeviceCount(InputDevice::Keyboard) > 0)
            _keyboard = te_new<Keyboard>("Keyboard", this);

        if (GetDeviceCount(InputDevice::Mouse) > 0)
            _mouse = te_new<Mouse>("Mouse", this);

        UINT32 numGamepads = GetDeviceCount(InputDevice::Gamepad);
        for (UINT32 i = 0; i < numGamepads; i++)
            _gamepads.push_back(te_new<GamePad>(_platformData->GamepadInfos[i].Name, _platformData->GamepadInfos[i], this));
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

        if (_platformData->DirectInput)
        {
            _platformData->DirectInput->Release();
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