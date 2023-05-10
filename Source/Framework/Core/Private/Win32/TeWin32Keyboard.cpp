#include "Input/TeKeyboard.h"
#include "Input/TeInput.h"
#include "Private/Win32/TeWin32Input.h"

namespace te
{
    /** Contains private data for the Win32 Keyboard implementation. */
    struct Keyboard::Pimpl
    {
        IDirectInput8* DirectInput;
        IDirectInputDevice8* Keyboard;
        DWORD CoopSettings;
        HWND HWnd;

        UINT8 KeyBuffer[256];
    };

    /**
     * Initializes DirectInput keyboard device for a window with the specified handle. Only input from that window will be
     * reported.
     */
    void InitializeDirectInput(Keyboard::Pimpl* data, HWND hWnd)
    {
        DIPROPDWORD dipdw;
        dipdw.diph.dwSize = sizeof(DIPROPDWORD);
        dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        dipdw.diph.dwObj = 0;
        dipdw.diph.dwHow = DIPH_DEVICE;
        dipdw.dwData = DI_BUFFER_SIZE_KEYBOARD;

        if (FAILED(data->DirectInput->CreateDevice(GUID_SysKeyboard, &data->Keyboard, nullptr)))
            TE_ASSERT_ERROR(false, "DirectInput keyboard init: Failed to create device.");

        if (FAILED(data->Keyboard->SetDataFormat(&c_dfDIKeyboard)))
            TE_ASSERT_ERROR(false, "DirectInput keyboard init: Failed to set format.");

        if (FAILED(data->Keyboard->SetCooperativeLevel(hWnd, data->CoopSettings)))
            TE_ASSERT_ERROR(false, "DirectInput keyboard init: Failed to set coop level.");

        if (FAILED(data->Keyboard->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)))
            TE_ASSERT_ERROR(false, "DirectInput keyboard init: Failed to set property.");

        HRESULT hr = data->Keyboard->Acquire();
        if (FAILED(hr) && hr != DIERR_OTHERAPPHASPRIO)
            TE_ASSERT_ERROR(false, "DirectInput keyboard init: Failed to acquire device.");

        data->HWnd = hWnd;
    }

    /** Releases DirectInput resources for the provided device */
    void ReleaseDirectInput(Keyboard::Pimpl* data)
    {
        if (data->Keyboard)
        {
            data->Keyboard->Unacquire();
            data->Keyboard->Release();
            data->Keyboard = nullptr;
        }
    }

    Keyboard::Keyboard(const String& name, Input* owner)
        : _name(name), _owner(owner)
    {
        InputPrivateData* pvtData = owner->GetPrivateData();

        _data = te_new<Pimpl>();
        _data->DirectInput = pvtData->DirectInput;
        _data->CoopSettings = pvtData->KbSettings;
        _data->Keyboard = nullptr;
        te_zero_out(_data->KeyBuffer);

        InitializeDirectInput(_data, (HWND)owner->GetWindowHandle());
    }

    Keyboard::~Keyboard()
    {
        ReleaseDirectInput(_data);
        te_delete(_data);
    }

    void Keyboard::Capture()
    {
        if (_data->Keyboard == nullptr)
            return;

        DIDEVICEOBJECTDATA diBuff[DI_BUFFER_SIZE_KEYBOARD];
        DWORD numEntries = DI_BUFFER_SIZE_KEYBOARD;

        HRESULT hr = _data->Keyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), diBuff, &numEntries, 0);
        if (hr != DI_OK)
        {
            hr = _data->Keyboard->Acquire();
            if (hr == E_ACCESSDENIED)
            {
                TE_DEBUG("Keyboard access denied");
            }
            
            while (hr == DIERR_INPUTLOST)
                hr = _data->Keyboard->Acquire();

            return;
        }

        if (FAILED(hr))
        {
            TE_DEBUG("Failed to read keyboard input. Internal error. ");
            return;
        }

        for (UINT32 i = 0; i < numEntries; ++i)
        {
            ButtonCode buttonCode = (ButtonCode)diBuff[i].dwOfs;

            _data->KeyBuffer[buttonCode] = (UINT8)(diBuff[i].dwData);

            if (diBuff[i].dwData & 0x80)
            {
                _owner->NotifyButtonPressed(0, buttonCode, diBuff[i].dwTimeStamp);
            }   
            else
            {
                _owner->NotifyButtonReleased(0, buttonCode, diBuff[i].dwTimeStamp);
            }
        }
    }

    void Keyboard::ChangeCaptureContext(UINT64 windowHandle)
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
