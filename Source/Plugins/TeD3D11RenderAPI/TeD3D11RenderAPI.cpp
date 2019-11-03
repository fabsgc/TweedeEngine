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

        ID3D11Device* device = nullptr;
        _device = te_new<D3D11Device>(device);
    }

    void D3D11RenderAPI::Update()
    {
    }
}