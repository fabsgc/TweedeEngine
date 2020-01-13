#include "TeD3D11RenderWindow.h"
#include "Private/Win32/TeWin32Platform.h"
#include "TeD3D11VideoModeInfo.h"
#include "TeD3D11RenderAPI.h"
#include "Image/TeTexture.h"

namespace te
{
    D3D11RenderWindow::D3D11RenderWindow(const RENDER_WINDOW_DESC& desc, D3D11Device& device, IDXGIFactory1* DXGIFactory)
        : RenderWindow(desc)
        , _swapChainDesc()
        , _device(device)
        , _DXGIFactory(DXGIFactory)
        , _multisampleType()
    {
    }

    D3D11RenderWindow::~D3D11RenderWindow()
    {
        RenderWindowProperties& props = _properties;

        if (props.IsFullScreen)
        {
            _swapChain->SetFullscreenState(false, nullptr);
        }

        SAFE_RELEASE(_swapChain);

        if (_window != nullptr)
        {
            _window->Destroy();
            te_delete(_window);
        }

        DestroySizeDependedD3DResources();
    }

    void D3D11RenderWindow::Initialize()
    {
        ZeroMemory(&_swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

        _multisampleType.Count = 1;
        _multisampleType.Quality = 0;

        WINDOW_DESC windowDesc;
        windowDesc.ShowTitleBar = _desc.ShowTitleBar;
        windowDesc.ShowBorder = _desc.ShowBorder;
        windowDesc.AllowResize = _desc.AllowResize;
        windowDesc.EnableDoubleClick = true;
        windowDesc.Fullscreen = _desc.Fullscreen;
        windowDesc.Width = _desc.Mode.GetWidth();
        windowDesc.Height = _desc.Mode.GetHeight();
        windowDesc.Hidden = _desc.Hidden || _desc.HideUntilSwap;
        windowDesc.Left = _desc.Left;
        windowDesc.Top = _desc.Top;
        windowDesc.OuterDimensions = false;
        windowDesc.Title = _desc.Title;
        windowDesc.CreationParams = this;
        windowDesc.WndProc = &Win32Platform::_win32WndProc;
        
#ifdef TE_STATIC_LIB
        windowDesc.Module = GetModuleHandle(NULL);
#else
        windowDesc.Module = GetModuleHandle("TeD3D11RenderAPI.dll");
#endif

        const D3D11VideoMode& d3d11videoMode = static_cast<const D3D11VideoMode&>(_desc.Mode);
        _refreshRateNumerator = d3d11videoMode.GetRefreshRateNumerator();
        _refreshRateDenominator = d3d11videoMode.GetRefreshRateDenominator();

        const D3D11VideoOutputInfo* outputInfo = nullptr;

        const D3D11VideoModeInfo& videoModeInfo = static_cast<const D3D11VideoModeInfo&>(RenderAPI::Instance().GetVideoModeInfo());
        UINT32 numOutputs = videoModeInfo.GetNumOutputs();
        if (numOutputs > 0)
        {
            UINT32 actualMonitorIdx = std::min(_desc.Mode.GetOutputIdx(), numOutputs - 1);
            outputInfo = static_cast<const D3D11VideoOutputInfo*>(&videoModeInfo.GetOutputInfo(actualMonitorIdx));

            DXGI_OUTPUT_DESC desc;
            outputInfo->GetDXGIOutput()->GetDesc(&desc);

            windowDesc.Monitor = desc.Monitor;
        }

        _window = te_new<Win32Window>(windowDesc);

        _properties.Width = _window->GetWidth();
        _properties.Height = _window->GetHeight();
        _properties.Top = _window->GetTop();
        _properties.Left = _window->GetLeft();

        CreateSwapChain();

        _properties.IsFullScreen = _desc.Fullscreen;
        _properties.IsWindow = true;

        if (_properties.IsFullScreen)
        {
            if (outputInfo != nullptr)
            {
                _swapChain->SetFullscreenState(true, outputInfo->GetDXGIOutput());
            }
            else
            {
                _swapChain->SetFullscreenState(true, nullptr);
            }
        }

        CreateSizeDependedD3DResources();
        _DXGIFactory->MakeWindowAssociation(_window->GetHWnd(), NULL);

        RenderWindow::Initialize();
    }

    void D3D11RenderWindow::GetCustomAttribute(const String& name, void* pData) const
    {
        if (name == "WINDOW")
        {
            UINT64 *pWnd = (UINT64*)pData;
            *pWnd = (UINT64)_window->GetHWnd();
            return;
        }

        if (name == "RTV")
        {
            *static_cast<ID3D11RenderTargetView**>(pData) = _renderTargetView;
            return;
        }
        else if (name == "DSV")
        {
            // TODO
            return;
        }
        else if (name == "RODSV")
        {
            // TODO
            return;
        }
        else if (name == "RODWSV")
        {
            // TODO
            return;
        }
        else if (name == "WDROSV")
        {
            // TODO
            return;
        }

        RenderWindow::GetCustomAttribute(name, pData);
    }

    HWND D3D11RenderWindow::GetHWnd() const
    {
        return _window->GetHWnd();
    }

    void D3D11RenderWindow::Move(INT32 left, INT32 top)
    {
        if (!_properties.IsFullScreen)
        {
            _window->Move(left, top);

            _properties.Top = _window->GetTop();
            _properties.Left = _window->GetLeft();
        }
    }

    void D3D11RenderWindow::Resize(UINT32 width, UINT32 height)
    {
        if (!_properties.IsFullScreen)
        {
            _window->Resize(width, height);

            _properties.Width = _window->GetWidth();
            _properties.Height = _window->GetHeight();
        }
    }

    void D3D11RenderWindow::SetHidden(bool hidden)
    {
        _window->SetHidden(hidden);

        RenderWindow::SetHidden(hidden);
    }

    void D3D11RenderWindow::SetActive(bool state)
    {
        _window->SetActive(state);

        if (_swapChain)
        {
            if (state)
            {
                _swapChain->SetFullscreenState(_properties.IsFullScreen, nullptr);
            }
            else
            {
                _swapChain->SetFullscreenState(FALSE, nullptr);
            }
        }
    }

    void D3D11RenderWindow::Minimize()
    {
        _window->Minimize();
    }

    void D3D11RenderWindow::Maximize()
    {
        _window->Maximize();
    }

    void D3D11RenderWindow::Restore()
    {
        _window->Restore();
    }

    void D3D11RenderWindow::SetFullscreen(UINT32 width, UINT32 height, float refreshRate, UINT32 monitorIdx)
    {
        const D3D11VideoModeInfo& videoModeInfo = static_cast<const D3D11VideoModeInfo&>(RenderAPI::Instance().GetVideoModeInfo());
		UINT32 numOutputs = videoModeInfo.GetNumOutputs();
		if (numOutputs == 0)
        {
			return;
        }

		UINT32 actualMonitorIdx = std::min(monitorIdx, numOutputs - 1);
		const D3D11VideoOutputInfo& outputInfo = static_cast<const D3D11VideoOutputInfo&>(videoModeInfo.GetOutputInfo(actualMonitorIdx));

		DXGI_MODE_DESC modeDesc;
		ZeroMemory(&modeDesc, sizeof(modeDesc));

		modeDesc.Width = width;
		modeDesc.Height = height;
		modeDesc.RefreshRate.Numerator = Math::RoundToInt(refreshRate);
		modeDesc.RefreshRate.Denominator = 1;
		modeDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		modeDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		modeDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		DXGI_MODE_DESC nearestMode;
		ZeroMemory(&nearestMode, sizeof(nearestMode));

		outputInfo.GetDXGIOutput()->FindClosestMatchingMode(&modeDesc, &nearestMode, nullptr);

		_properties.IsFullScreen = true;
		_properties.Width = width;
		_properties.Height = height;

		_swapChain->ResizeTarget(&nearestMode);
		_swapChain->SetFullscreenState(true, outputInfo.GetDXGIOutput());

		NotifyMovedOrResized();
    }

    void D3D11RenderWindow::SetFullscreen(const VideoMode& mode)
    {
        const D3D11VideoModeInfo& videoModeInfo = static_cast<const D3D11VideoModeInfo&>(RenderAPI::Instance().GetVideoModeInfo());
        UINT32 numOutputs = videoModeInfo.GetNumOutputs();
        if (numOutputs == 0)
        {
            return;
        }

        UINT32 actualMonitorIdx = std::min(mode.GetOutputIdx(), numOutputs - 1);
        const D3D11VideoOutputInfo& outputInfo = static_cast<const D3D11VideoOutputInfo&>(videoModeInfo.GetOutputInfo(actualMonitorIdx));

        const D3D11VideoMode& videoMode = static_cast<const D3D11VideoMode&>(mode);

        _properties.IsFullScreen = true;
        _properties.Width = mode.GetWidth();
        _properties.Height = mode.GetHeight();

        _swapChain->ResizeTarget(&videoMode.GetDXGIModeDesc());
        _swapChain->SetFullscreenState(true, outputInfo.GetDXGIOutput());

        NotifyMovedOrResized();
    }

    void D3D11RenderWindow::SetWindowed(UINT32 width, UINT32 height)
    {
        _properties.Width = width;
        _properties.Height = height;
        _properties.IsFullScreen = false;

        _swapChainDesc.Windowed = true;
        _swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        _swapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
        _swapChainDesc.BufferDesc.Width = width;
        _swapChainDesc.BufferDesc.Height = height;

        DXGI_MODE_DESC modeDesc;
        ZeroMemory(&modeDesc, sizeof(modeDesc));

        modeDesc.Width = width;
        modeDesc.Height = height;
        modeDesc.RefreshRate.Numerator = 0;
        modeDesc.RefreshRate.Denominator = 0;
        modeDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

        _swapChain->SetFullscreenState(false, nullptr);
        _swapChain->ResizeTarget(&modeDesc);

        NotifyMovedOrResized();
    }

    Vector2I D3D11RenderWindow::ScreenToWindowPos(const Vector2I& screenPos) const
    {
        POINT pos;
        pos.x = screenPos.x;
        pos.y = screenPos.y;

        ScreenToClient(GetHWnd(), &pos);
        return Vector2I(pos.x, pos.y);
    }

    Vector2I D3D11RenderWindow::WindowToScreenPos(const Vector2I& windowPos) const
    {
        POINT pos;
        pos.x = windowPos.x;
        pos.y = windowPos.y;

        ClientToScreen(GetHWnd(), &pos);
        return Vector2I(pos.x, pos.y);
    }

    void D3D11RenderWindow::WindowMovedOrResized()
    {
        if (!_window)
            return;

        _window->WindowMovedOrResized();

        if (_properties.IsFullScreen) // Fullscreen is handled directly by this object
        {
            ResizeSwapChainBuffers(_properties.Width, _properties.Height);
        }
        else
        {
            ResizeSwapChainBuffers(_window->GetWidth(), _window->GetHeight());
            _properties.Width = _window->GetWidth();
            _properties.Height = _window->GetHeight();
            _properties.Top = _window->GetTop();
            _properties.Left = _window->GetLeft();
        }
    }

    void D3D11RenderWindow::CreateSizeDependedD3DResources()
    {
        SAFE_RELEASE(_backBuffer);

        HRESULT hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&_backBuffer);
        if (FAILED(hr))
        {
            TE_ASSERT_ERROR(false, "Unable to Get Back Buffer for swap chain", __FILE__, __LINE__);
        }

        assert(_backBuffer && !_renderTargetView);

        D3D11_TEXTURE2D_DESC BBDesc;
        _backBuffer->GetDesc(&BBDesc);

        D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
        ZeroMemory(&RTVDesc, sizeof(RTVDesc));

        RTVDesc.Format = BBDesc.Format;
        RTVDesc.ViewDimension = GetProperties().MultisampleCount > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
        RTVDesc.Texture2D.MipSlice = 0;
        hr = _device.GetD3D11Device()->CreateRenderTargetView(_backBuffer, &RTVDesc, &_renderTargetView);

        if (FAILED(hr))
        {
            String errorDescription = _device.GetErrorDescription();
            TE_ASSERT_ERROR(false, "Unable to create rendertarget view\nError Description:" + errorDescription, __FILE__, __LINE__);
        }

        //_depthStencilView = nullptr; TODO

        if (_desc.DepthBuffer)
        {
            TEXTURE_DESC texDesc;
            texDesc.Type = TEX_TYPE_2D;
            texDesc.Width = BBDesc.Width;
            texDesc.Height = BBDesc.Height;
            texDesc.Format = PF_D32_S8X24;
            texDesc.Usage = TU_DEPTHSTENCIL;
            texDesc.NumSamples = GetProperties().MultisampleCount;

            _depthStencilBuffer = Texture::_createPtr(texDesc);
            // _depthStencilView = _depthStencilBuffer->requestView(0, 1, 0, 1, GVU_DEPTHSTENCIL); TODO
        }
        else
        {
            _depthStencilBuffer = nullptr;
        }
    }

    void D3D11RenderWindow::DestroySizeDependedD3DResources()
    {
        SAFE_RELEASE(_backBuffer);
        SAFE_RELEASE(_renderTargetView);

        _depthStencilBuffer = nullptr;
    }

    void D3D11RenderWindow::ResizeSwapChainBuffers(UINT32 width, UINT32 height)
    {
        DestroySizeDependedD3DResources();

        UINT Flags = _properties.IsFullScreen ? DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : 0;
        HRESULT hr = _swapChain->ResizeBuffers(_swapChainDesc.BufferCount, width, height, _swapChainDesc.BufferDesc.Format, Flags);

        if (hr != S_OK)
        {
            TE_ASSERT_ERROR(false, "Call to ResizeBuffers failed.", __FILE__, __LINE__);
        }

        _swapChain->GetDesc(&_swapChainDesc);
        _properties.Width = _swapChainDesc.BufferDesc.Width;
        _properties.Height = _swapChainDesc.BufferDesc.Height;
        _properties.IsFullScreen = (0 == _swapChainDesc.Windowed); // Alt-Enter together with SetWindowAssociation() can change this state

        CreateSizeDependedD3DResources();

        _device.GetImmediateContext()->OMSetRenderTargets(0, 0, 0);
    }

    void D3D11RenderWindow::CreateSwapChain()
    {
        ZeroMemory(&_swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

        RenderWindowProperties& props = _properties;
        IDXGIDevice* pDXGIDevice = QueryDxgiDevice();

        ZeroMemory(&_swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
        DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
        _swapChainDesc.OutputWindow = _window->GetHWnd();
        _swapChainDesc.BufferDesc.Width = props.Width;
        _swapChainDesc.BufferDesc.Height = props.Height;
        _swapChainDesc.BufferDesc.Format = format;

        if (props.IsFullScreen)
        {
            _swapChainDesc.BufferDesc.RefreshRate.Numerator = _refreshRateNumerator;
            _swapChainDesc.BufferDesc.RefreshRate.Denominator = _refreshRateDenominator;
        }
        else
        {
            _swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
            _swapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
        }

        _swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        _swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        _swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        _swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        _swapChainDesc.BufferCount = 1;
        _swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        _swapChainDesc.Windowed = true;

        D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::InstancePtr());
        rs->DetermineMultisampleSettings(props.MultisampleCount, format, &_multisampleType);
        _swapChainDesc.SampleDesc.Count = _multisampleType.Count;
        _swapChainDesc.SampleDesc.Quality = _multisampleType.Quality;

        HRESULT hr;

        // Create swap chain			
        hr = _DXGIFactory->CreateSwapChain(pDXGIDevice, &_swapChainDesc, &_swapChain);

        if (FAILED(hr))
        {
            // Try a second time, may fail the first time due to back buffer count,
            // which will be corrected by the runtime
            hr = _DXGIFactory->CreateSwapChain(pDXGIDevice, &_swapChainDesc, &_swapChain);
        }

        SAFE_RELEASE(pDXGIDevice);

        if (FAILED(hr))
        {
            TE_ASSERT_ERROR(false, "Unable to create swap chain. Error code: " + ToString(hr), __FILE__, __LINE__);
        }
    }

    IDXGIDevice* D3D11RenderWindow::QueryDxgiDevice()
	{
        TE_ASSERT_ERROR(_device.GetD3D11Device() != nullptr, "D3D11Device is null.", __FILE__, __LINE__);

        IDXGIDevice* pDXGIDevice = nullptr;
        HRESULT hr = _device.GetD3D11Device()->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice);

        if (FAILED(hr))
        {
            TE_ASSERT_ERROR(false, "Unable to query a DXGIDevice.", __FILE__, __LINE__);
        }

        return pDXGIDevice;
    }

    void D3D11RenderWindow::SwapBuffers()
    {
        if (_device.GetD3D11Device() != nullptr)
        {
            HRESULT hr = _swapChain->Present(GetProperties().VSync ? 1 : 0, 0);

            if (FAILED(hr))
            {
                TE_ASSERT_ERROR(false, "Error Presenting surfaces", __FILE__, __LINE__);
            }
        }
    }

    void D3D11RenderWindow::SetTitle(const String& title)
    {
        SetWindowText(GetHWnd(), title.c_str());
    }
}