#include "Audio/TeAudioManager.h"
#include "Audio/TeAudio.h"

#include <Mmdeviceapi.h>

namespace te
{
    class CMMNotificationClient : public IMMNotificationClient
    {
    public:
        CMMNotificationClient() 
            : _cRef(1)
        { }

        ~CMMNotificationClient()
        { }

        ULONG STDMETHODCALLTYPE AddRef()
        {
            return InterlockedIncrement(&_cRef);
        }

        ULONG STDMETHODCALLTYPE Release()
        {
            ULONG ulRef = InterlockedDecrement(&_cRef);
            if (0 == ulRef)
            {
                delete this;
            }
            return ulRef;
        }

        HRESULT STDMETHODCALLTYPE QueryInterface(
            REFIID riid, VOID **ppvInterface)
        {
            if (IID_IUnknown == riid)
            {
                AddRef();
                *ppvInterface = (IUnknown*)this;
            }
            else if (__uuidof(IMMNotificationClient) == riid)
            {
                AddRef();
                *ppvInterface = (IMMNotificationClient*)this;
            }
            else
            {
                *ppvInterface = NULL;
                return E_NOINTERFACE;
            }
            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDeviceId)
        {
            switch (flow)
            {
            case eRender:
                gAudio().OnDefaultDeviceChanged();
                break;
            }

            return S_OK;
        }
        HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR pwstrDeviceId)
        {
            gAudio().OnDeviceAdded();
            return S_OK;
        };

        HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR pwstrDeviceId)
        {
            gAudio().OnDeviceRemoved();
            return S_OK;
        }
        HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState)
        {
            gAudio().OnDeviceStateChanged();
            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key)
        {
            return S_OK;
        }

    private:
        LONG _cRef;
    };

    struct AudioDeviceWatcher::Pimpl
    {
        IMMDeviceEnumerator* DeviceEnumerator = nullptr;
        CMMNotificationClient* NotificationClient = nullptr;
    };

    AudioDeviceWatcher::AudioDeviceWatcher()
    {
        _data = te_new<Pimpl>();

        if (!_data->DeviceEnumerator)
        {
            HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,
                CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&_data->DeviceEnumerator);

            if (SUCCEEDED(hr))
            {
                _data->NotificationClient = new CMMNotificationClient();
                _data->DeviceEnumerator->RegisterEndpointNotificationCallback(_data->NotificationClient);
            }
        }
    }

    AudioDeviceWatcher::~AudioDeviceWatcher()
    {
        if (_data->DeviceEnumerator)
        {
            _data->DeviceEnumerator->UnregisterEndpointNotificationCallback(_data->NotificationClient);
            _data->DeviceEnumerator->Release();
        }

        te_delete(_data);
    }
}
