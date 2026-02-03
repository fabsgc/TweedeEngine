#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "RenderAPI/TeRenderWindow.h"

namespace te
{
    class D3D11HeadlessRenderWindow : public RenderWindow
    {
    public:
        D3D11HeadlessRenderWindow(const RENDER_WINDOW_DESC& desc, D3D11Device& device, IDXGIFactory1* DXGIFactory);
        virtual ~D3D11HeadlessRenderWindow()  {};

        /** @copydoc RenderWindow::Initialize */
        void Initialize() override;

        /** @copydoc RenderWindow::InitializeGui */
        void InitializeGui() const override;

        /** @copydoc RenderWindow::GetCustomAttribute */
        void GetCustomAttribute(const String& name, void* pData) const override;

        /** @copydoc RenderWindow::SwapBuffers */
        void SwapBuffers() override;

    private:
        D3D11Device& _device;
        IDXGIFactory1* _DXGIFactory;

        SPtr<Texture> _backBuffer = nullptr;
        ID3D11RenderTargetView* _renderTargetView = nullptr;

        SPtr<TextureView> _depthStencilView = nullptr;
        SPtr<Texture> _depthStencilBuffer = nullptr;
    };
}
