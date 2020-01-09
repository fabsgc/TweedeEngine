#include "TeD3D11RenderAPI.h"
#include "TeD3D11RenderWindow.h"

namespace te
{
    D3D11RenderAPI::D3D11RenderAPI()
        : _viewport()
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

        // Create & register HLSL factory		
        _HLSLFactory = te_new<D3D11HLSLProgramFactory>();

        // Create Input Layout Manager	
        _IAManager = te_new<D3D11InputLayoutManager>();
    }

    void D3D11RenderAPI::Destroy()
    {
        // Ensure that all GPU commands finish executing before shutting down the device. If we don't do this a crash
        // on shutdown may occurr as the driver is still executing the commands, and we unload this library.
        _device->GetImmediateContext()->Flush();

        if (_IAManager != nullptr)
        {
            te_delete(_IAManager);
            _IAManager = nullptr;
        }

        if (_HLSLFactory != nullptr)
        {
            te_delete(_HLSLFactory);
            _HLSLFactory = nullptr;
        }

        SAFE_RELEASE(_DXGIFactory);

        if (_device != nullptr)
        {
            te_delete(_device);
            _device = nullptr;
        }

        if (_driverList != nullptr)
        {
            te_delete(_driverList);
            _driverList = nullptr;
        }

        _activeD3DDriver = nullptr;

        _activeVertexDeclaration = nullptr;

        RenderAPI::Destroy();
    }

    void D3D11RenderAPI::SetViewport(const Rect2& area)
    {
        _viewportNorm = area;
        ApplyViewport();
    }

    void D3D11RenderAPI::SetVertexBuffers(UINT32 index, SPtr<VertexBuffer>* buffers, UINT32 numBuffers)
    {
        // TODO
    }

    void D3D11RenderAPI::SetIndexBuffer(const SPtr<IndexBuffer>& buffer)
    {
        // TODO
    }

    void D3D11RenderAPI::Draw(UINT32 vertexOffset, UINT32 vertexCount, UINT32 instanceCount)
    {
        // TODO
    }

    void D3D11RenderAPI::DrawIndexed(UINT32 startIndex, UINT32 indexCount, UINT32 vertexOffset, UINT32 vertexCount, UINT32 instanceCount)
    {
        // TODO
    }

    void D3D11RenderAPI::SwapBuffers(const SPtr<RenderTarget>& target)
    {
        target->SwapBuffers();
    }

    void D3D11RenderAPI::SetRenderTarget(const SPtr<RenderTarget>& target)
    {
        // TODO
    }

    void D3D11RenderAPI::ClearRenderTarget(UINT32 buffers, float depth, UINT16 stencil, UINT8 targetMask)
    {
        // TODO
    }

    void D3D11RenderAPI::ClearViewport(UINT32 buffers, float depth, UINT16 stencil, UINT8 targetMask)
    {
        // TODO
    }

    void D3D11RenderAPI::ApplyInputLayout()
    {
        if (_activeVertexDeclaration == nullptr)
        {
            TE_DEBUG("Cannot apply input layout without a vertex declaration. Set vertex declaration before calling this method.", __FILE__, __LINE__);
            return;
        }

        /*if (_activeVertexShader == nullptr)
        {
            TE_DEBUG("Cannot apply input layout without a vertex shader. Set vertex shader before calling this method.", __FILE__, __LINE__);
            return;
        }*/

        ID3D11InputLayout* ia = NULL; // TODO
        _device->GetImmediateContext()->IASetInputLayout(ia);
    }

    void D3D11RenderAPI::ApplyViewport()
    {
        if (_activeRenderTarget == nullptr)
        {
            return;
        }

        const RenderTargetProperties& rtProps = _activeRenderTarget->GetProperties();

        // Set viewport dimensions
        _viewport.TopLeftX = (FLOAT)(rtProps.Width * _viewportNorm.x);
        _viewport.TopLeftY = (FLOAT)(rtProps.Height * _viewportNorm.y);
        _viewport.Width = (FLOAT)(rtProps.Width * _viewportNorm.width);
        _viewport.Height = (FLOAT)(rtProps.Height * _viewportNorm.height);

        if (rtProps.RequiresTextureFlipping)
        {
            // Convert "top-left" to "bottom-left"
            _viewport.TopLeftY = rtProps.Height - _viewport.Height - _viewport.TopLeftY;
        }

        _viewport.MinDepth = 0.0f;
        _viewport.MaxDepth = 1.0f;

        _device->GetImmediateContext()->RSSetViewports(1, &_viewport);
    }

    void D3D11RenderAPI::NotifyRenderTargetModified()
    {
        if (_activeRenderTarget == nullptr || _activeRenderTargetModified)
        {
            return;
        }

        //_activeRenderTarget->_tickUpdateCount();
        _activeRenderTargetModified = true;
    }
}
