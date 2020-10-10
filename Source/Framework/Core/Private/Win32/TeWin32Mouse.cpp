#include "Input/TeMouse.h"
#include "Input/TeInput.h"
#include "Private/Win32/TeWin32Input.h"

namespace te
{
    /** Contains private data for the Win32 Mouse implementation. */
    struct Mouse::Pimpl
    {
        IDirectInput8* DirectInput;
        IDirectInputDevice8* Mouse;
        DWORD CoopSettings;
        HWND HWnd;
    };

    /** Initializes DirectInput mouse device for a window with the specified handle. Only input from that window will be reported. */
    void InitializeDirectInput(Mouse::Pimpl* data, HWND hWnd)
    {
        DIPROPDWORD dipdw;
        dipdw.diph.dwSize = sizeof(DIPROPDWORD);
        dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        dipdw.diph.dwObj = 0;
        dipdw.diph.dwHow = DIPH_DEVICE;
        dipdw.dwData = DI_BUFFER_SIZE_MOUSE;

        if (FAILED(data->DirectInput->CreateDevice(GUID_SysMouse, &data->Mouse, nullptr)))
            TE_ASSERT_ERROR(false, "DirectInput mouse init: Failed to create device.");

        if (FAILED(data->Mouse->SetDataFormat(&c_dfDIMouse2)))
            TE_ASSERT_ERROR(false, "DirectInput mouse init: Failed to set format.");

        if (FAILED(data->Mouse->SetCooperativeLevel(hWnd, data->CoopSettings)))
            TE_ASSERT_ERROR(false, "DirectInput mouse init: Failed to set coop level.");

        if (FAILED(data->Mouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)))
            TE_ASSERT_ERROR(false, "DirectInput mouse init: Failed to set property.");

        HRESULT hr = data->Mouse->Acquire();
        if (FAILED(hr) && hr != DIERR_OTHERAPPHASPRIO)
            TE_ASSERT_ERROR(false, "DirectInput mouse init: Failed to acquire device.");

        data->HWnd = hWnd;
    }

    /** Releases DirectInput resources for the provided device */
    void ReleaseDirectInput(Mouse::Pimpl* data)
    {
        if (data->Mouse)
        {
            data->Mouse->Unacquire();
            data->Mouse->Release();
            data->Mouse = nullptr;
        }
    }

    /** Notifies the input handler that a mouse press or release occurred. Triggers an event in the input handler. */
    void DoMouseClick(Input* owner, ButtonCode mouseButton, const DIDEVICEOBJECTDATA& data)
    {
        if (data.dwData & 0x80)
            owner->NotifyButtonPressed(0, mouseButton, data.dwTimeStamp);
        else
            owner->NotifyButtonReleased(0, mouseButton, data.dwTimeStamp);
    }

    Mouse::Mouse(const String& name, Input* owner)
        : _name(name)
        , _owner(owner)
    {
        InputPrivateData* pvtData = owner->GetPrivateData();

        _data = te_new<Pimpl>();
        _data->DirectInput = pvtData->DirectInput;
        _data->CoopSettings = pvtData->MouseSettings;
        _data->Mouse = nullptr;

        InitializeDirectInput(_data, (HWND)owner->GetWindowHandle());
    }

    Mouse::~Mouse()
    {
        ReleaseDirectInput(_data);
        te_delete(_data);
    }

    void Mouse::Capture()
    {
        if (_data->Mouse == nullptr)
            return;

        DIDEVICEOBJECTDATA diBuff[DI_BUFFER_SIZE_MOUSE];
        DWORD numEntries = DI_BUFFER_SIZE_MOUSE;

        HRESULT hr = _data->Mouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), diBuff, &numEntries, 0);
        if (hr != DI_OK)
        {
            hr = _data->Mouse->Acquire();
            while (hr == DIERR_INPUTLOST)
                hr = _data->Mouse->Acquire();

            hr = _data->Mouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), diBuff, &numEntries, 0);

            if (FAILED(hr))
                return;
        }

        INT32 relX, relY, relZ;
        relX = relY = relZ = 0;

        bool axesMoved = false;
        for (UINT32 i = 0; i < numEntries; ++i)
        {
            switch (diBuff[i].dwOfs)
            {
            case DIMOFS_BUTTON0:
                DoMouseClick(_owner, TE_MOUSE_LEFT, diBuff[i]);
                break;
            case DIMOFS_BUTTON1:
                DoMouseClick(_owner, TE_MOUSE_RIGHT, diBuff[i]);
                break;
            case DIMOFS_BUTTON2:
                DoMouseClick(_owner, TE_MOUSE_MIDDLE, diBuff[i]);
                break;
            case DIMOFS_BUTTON3:
                DoMouseClick(_owner, TE_MOUSE_BTN4, diBuff[i]);
                break;
            case DIMOFS_BUTTON4:
                DoMouseClick(_owner, TE_MOUSE_BTN5, diBuff[i]);
                break;
            case DIMOFS_BUTTON5:
                DoMouseClick(_owner, TE_MOUSE_BTN6, diBuff[i]);
                break;
            case DIMOFS_BUTTON6:
                DoMouseClick(_owner, TE_MOUSE_BTN7, diBuff[i]);
                break;
            case DIMOFS_BUTTON7:
                DoMouseClick(_owner, TE_MOUSE_BTN8, diBuff[i]);
                break;
            case DIMOFS_X:
                relX += diBuff[i].dwData;
                axesMoved = true;
                break;
            case DIMOFS_Y:
                relY += diBuff[i].dwData;
                axesMoved = true;
                break;
            case DIMOFS_Z:
                relZ += diBuff[i].dwData;
                axesMoved = true;
                break;
            default: break;
            }
        }

        if (axesMoved)
            _owner->NotifyMouseMoved(relX, relY, relZ);
    }

    void Mouse::ChangeCaptureContext(UINT64 windowHandle)
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