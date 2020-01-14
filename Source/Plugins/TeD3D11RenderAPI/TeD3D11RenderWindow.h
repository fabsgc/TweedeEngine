#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "RenderAPI/TeRenderWindow.h"
#include "Private/Win32/TeWin32Window.h"
#include "Math/TeVector2I.h"
#include "TeD3D11Device.h"

namespace te
{
    class D3D11RenderWindow : public RenderWindow
	{
	public:
        D3D11RenderWindow(const RENDER_WINDOW_DESC& desc, D3D11Device& device, IDXGIFactory1* DXGIFactory);
		~D3D11RenderWindow();

        void Initialize() override;
        void GetCustomAttribute(const String& name, void* pData) const override;
        void WindowMovedOrResized() override;

        /**	Retrieves internal window handle. */
        HWND GetHWnd() const;

        /** @copydoc RenderWindow::Move */
        void Move(INT32 left, INT32 top) override;

        /** @copydoc RenderWindow::Resize */
        void Resize(UINT32 width, UINT32 height) override;

        /** @copydoc RenderWindow::SetHidden */
        void SetHidden(bool hidden) override;

        /** @copydoc RenderWindow::SetActive */
        void SetActive(bool state) override;

        /** @copydoc RenderWindow::Minimize */
        void Minimize() override;

        /** @copydoc RenderWindow::Maximize */
        void Maximize() override;

        /** @copydoc RenderWindow::Restore */
        void Restore() override;

        /** @copydoc RenderWindow::SetFullscreen(UINT32, UINT32, float, UINT32) */
        void SetFullscreen(UINT32 width, UINT32 height, float refreshRate = 60.0f, UINT32 monitorIdx = 0) override;

        /** @copydoc RenderWindow::SetFullscreen(const VideoMode&) */
        void SetFullscreen(const VideoMode& mode) override;

        /** @copydoc RenderWindow::SetWindowed */
        void SetWindowed(UINT32 width, UINT32 height) override;

        /** @copydoc RenderWindow::ScreenToWindowPos */
        Vector2I ScreenToWindowPos(const Vector2I& screenPos) const override;

        /** @copydoc RenderWindow::WindowToScreenPos */
        Vector2I WindowToScreenPos(const Vector2I& windowPos) const override;

        /** @copydoc RenderWindow::SwapBuffers */
        void SwapBuffers() override;

        /** @copydoc RenderWindow::SetTitle */
        void SetTitle(const String& title) override;

    protected:
        /**	Creates internal resources dependent on window size. */
        void CreateSizeDependedD3DResources();

        /**	Destroys internal resources dependent on window size. */
        void DestroySizeDependedD3DResources();

        /**	Creates a swap chain for the window. */
        void CreateSwapChain();

        /**	Queries the current DXGI device. Make sure to release the returned object when done with it. */
        IDXGIDevice* QueryDxgiDevice();

        /**	Resizes all buffers attached to the swap chain to the specified size. */
        void ResizeSwapChainBuffers(UINT32 width, UINT32 height);

    protected:
        D3D11Device& _device;
		IDXGIFactory1* _DXGIFactory;

        DXGI_SAMPLE_DESC _multisampleType;
		UINT32 _refreshRateNumerator = 0;
		UINT32 _refreshRateDenominator = 0;

        ID3D11Texture2D* _backBuffer = nullptr;
		ID3D11RenderTargetView* _renderTargetView = nullptr;
        SPtr<TextureView> _depthStencilView = nullptr;
        SPtr<Texture> _depthStencilBuffer;

        IDXGISwapChain* _swapChain = nullptr;
        DXGI_SWAP_CHAIN_DESC _swapChainDesc;
        Win32Window* _window = nullptr;
    };
}