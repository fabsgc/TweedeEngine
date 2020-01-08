#include "TeD3D11RenderAPI.h"
#include "TeD3D11RenderWindow.h"

namespace te
{
    D3D11RenderAPI::D3D11RenderAPI()
        : _viewport()
        , _scissorRect()
    {
    }

    D3D11RenderAPI::~D3D11RenderAPI()
    {
    }

    SPtr<RenderWindow> D3D11RenderAPI::CreateRenderWindow(const RENDER_WINDOW_DESC& windowDesc)
    {
        return te_shared_ptr_new<D3D11RenderWindow>(windowDesc, *_device, _DXGIFactory);
    }

    void D3D11RenderAPI::Initialize()
    {
        HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&_DXGIFactory);
        if (FAILED(hr))
        {
            TE_ASSERT_ERROR(false, "Failed to create Direct3D11 DXGIFactory", __FILE__, __LINE__);
        }

        _driverList = te_new<D3D11DriverList>(_DXGIFactory);
        _activeD3DDriver = _driverList->Item(0); // TODO: Always get first driver, for now
		_videoModeInfo = _activeD3DDriver->GetVideoModeInfo();

        IDXGIAdapter* selectedAdapter = _activeD3DDriver->GetDeviceAdapter();

        D3D_FEATURE_LEVEL requestedLevels[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1
        };

        const UINT32 numRequestedLevels = sizeof(requestedLevels) / sizeof(requestedLevels[0]);

        UINT32 deviceFlags = 0;
#if TE_DEBUG_MODE
        deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        ID3D11Device* device;
        hr = D3D11CreateDevice(selectedAdapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, deviceFlags,
            requestedLevels, numRequestedLevels, D3D11_SDK_VERSION, &device, &_featureLevel, nullptr);

        // This will fail on Win 7 due to lack of 11.1, so re-try again without it
        if (hr == E_INVALIDARG)
        {
            hr = D3D11CreateDevice(selectedAdapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, deviceFlags,
                &requestedLevels[1], numRequestedLevels - 1, D3D11_SDK_VERSION, &device, &_featureLevel, nullptr);
        }

        if (FAILED(hr))
        {
            TE_ASSERT_ERROR(false, "Failed to create Direct3D11 object. D3D11CreateDeviceN returned this error code: " + ToString(hr), __FILE__, __LINE__);
        }

        _device = te_new<D3D11Device>(device);
    }

    void D3D11RenderAPI::Update()
    {
    }
}
