#include "TeD3D11RenderWindow.h"
#include "Private/Win32/TeWin32Platform.h"

namespace te
{
    D3D11RenderWindow::D3D11RenderWindow(const RENDER_WINDOW_DESC& desc)
        : RenderWindow(desc)
        , _swapChainDesc()
    {
    }

    D3D11RenderWindow::~D3D11RenderWindow()
    {
        SAFE_RELEASE(_swapChain);

        if (_window != nullptr)
        {
            _window->Destroy();
            te_delete(_window);
        }
    }

    void D3D11RenderWindow::Update()
    {
    }

    void D3D11RenderWindow::Initialize()
    {
        ZeroMemory(&_swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

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

        _window = te_new<Win32Window>(windowDesc);

        _properties.Width = _window->GetWidth();
        _properties.Height = _window->GetHeight();
        _properties.Top = _window->GetTop();
        _properties.Left = _window->GetLeft();

        CreateSwapChain();

        _properties.IsFullScreen = _desc.Fullscreen;

        if (_properties.IsFullScreen)
        {
            /*if (outputInfo != nullptr)
                mSwapChain->SetFullscreenState(true, outputInfo->getDXGIOutput());
            else
                mSwapChain->SetFullscreenState(true, nullptr);*/
        }

        //createSizeDependedD3DResources();
        //mDXGIFactory->MakeWindowAssociation(mWindow->getHWnd(), NULL);

        //TODO
    }

    void D3D11RenderWindow::CreateDevice()
    {
        //TODO
    }
    void D3D11RenderWindow::CheckMSAASupport()
    {
        //TODO
    }

    void D3D11RenderWindow::CreateSwapChain()
    {
        //TODO
    }

    void D3D11RenderWindow::CreateDepthStencilBuffer()
    {
        //TODO
    }

    void D3D11RenderWindow::CreateViewport()
    {
        //TODO
    }

    void D3D11RenderWindow::SetPrimitiveTopology()
    {
        //TODO
    }

    void D3D11RenderWindow::SetBackfaceCulling()
    {
        //TODO
    }

    void D3D11RenderWindow::GetCustomAttribute(const String& name, void* pData) const
    {
        if (name == "WINDOW")
        {
            UINT64 *pWnd = (UINT64*)pData;
            *pWnd = (UINT64)_window->GetHWnd();
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
        //TODO
    }

    void D3D11RenderWindow::SetFullscreen(const VideoMode& videoMode)
    {
        //TODO
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
        //TODO
    }

    void D3D11RenderWindow::DestroySizeDependedD3DResources()
    {
        //TODO
    }

    void D3D11RenderWindow::ResizeSwapChainBuffers(UINT32 width, UINT32 height)
    {
        //TODO
    }
}