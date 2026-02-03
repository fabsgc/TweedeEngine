#include "TeD3D11HeadlessRenderWindow.h"
#include "TeD3D11TextureView.h"
#include "TeD3D11Device.h"
#include "TeD3D11Texture.h"
#include "Image/TeTexture.h"

namespace te
{
    D3D11HeadlessRenderWindow::D3D11HeadlessRenderWindow(const RENDER_WINDOW_DESC& desc, D3D11Device& device, IDXGIFactory1* DXGIFactory)
        : RenderWindow(desc)
        , _device(device)
        , _DXGIFactory(DXGIFactory)
    { }

    void D3D11HeadlessRenderWindow::Initialize()
    {
        RenderWindow::Initialize();

        _properties.IsWindow = true;

        // Create back buffer
        TEXTURE_DESC backBufferTexDesc;
        backBufferTexDesc.Type = TEX_TYPE_2D;
        backBufferTexDesc.Width = _desc.Mode.GetWidth();
        backBufferTexDesc.Height = _desc.Mode.GetHeight();
        backBufferTexDesc.Format = PF_RGBA8;
        backBufferTexDesc.Usage = TU_RENDERTARGET;
        backBufferTexDesc.NumSamples = GetProperties().MultisampleCount;
        backBufferTexDesc.DebugName = "Headless Back Buffer";

        _backBuffer = Texture::CreatePtr(backBufferTexDesc);

        SPtr<TextureView> texView = _backBuffer ? _backBuffer->RequestView(0, 1, 0, 1, GVU_RENDERTARGET, _backBuffer->GetProperties().GetDebugName()) : nullptr;
        _renderTargetView = texView ? static_cast<D3D11TextureView*>(texView.get())->GetRTV() : nullptr;
        
        if (!_renderTargetView)
        {
            String errorDescription = "Unable to create headless rendertarget view " + _device.GetErrorDescription();
            TE_ASSERT_ERROR(false, errorDescription.c_str());
        }

        if (_desc.DepthBuffer)
        {
            TEXTURE_DESC texDesc;
            texDesc.Type = TEX_TYPE_2D;
            texDesc.Width = _desc.Mode.GetWidth();
            texDesc.Height = _desc.Mode.GetHeight();
            texDesc.Format = PF_D32_S8X24;
            texDesc.Usage = TU_DEPTHSTENCIL;
            texDesc.NumSamples = GetProperties().MultisampleCount;
            texDesc.DebugName = "Headless Depth Stencil Buffer";

            _depthStencilBuffer = Texture::CreatePtr(texDesc);
            _depthStencilView = _depthStencilBuffer->RequestView(0, 1, 0, 1, GVU_DEPTHSTENCIL, texDesc.DebugName);
        }
    }

    void D3D11HeadlessRenderWindow::GetCustomAttribute(const String& name, void* pData) const
    {
        if (name == "RTV")
        {
            *static_cast<ID3D11RenderTargetView**>(pData) = _renderTargetView;
            return;
        }
        else if (name == "DSV")
        {
            if (_depthStencilView != nullptr)
            {
                D3D11TextureView* d3d11TextureView = static_cast<D3D11TextureView*>(_depthStencilView.get());
                *static_cast<ID3D11DepthStencilView**>(pData) = d3d11TextureView->GetDSV(false, false);
            }
            else
            {
                *static_cast<ID3D11DepthStencilView**>(pData) = nullptr;
            }

            return;
        }
        else if (name == "RODSV")
        {
            if (_depthStencilView != nullptr)
            {
                D3D11TextureView* d3d11TextureView = static_cast<D3D11TextureView*>(_depthStencilView.get());
                *static_cast<ID3D11DepthStencilView**>(pData) = d3d11TextureView->GetDSV(true, true);
            }
            else
            {
                *static_cast<ID3D11DepthStencilView**>(pData) = nullptr;
            }

            return;
        }
        else if (name == "RODWSV")
        {
            if (_depthStencilView != nullptr)
            {
                D3D11TextureView* d3d11TextureView = static_cast<D3D11TextureView*>(_depthStencilView.get());
                *static_cast<ID3D11DepthStencilView**>(pData) = d3d11TextureView->GetDSV(true, false);
            }
            else
            {
                *static_cast<ID3D11DepthStencilView**>(pData) = nullptr;
            }

            return;
        }
        else if (name == "WDROSV")
        {
            if (_depthStencilView != nullptr)
            {
                D3D11TextureView* d3d11TextureView = static_cast<D3D11TextureView*>(_depthStencilView.get());
                *static_cast<ID3D11DepthStencilView**>(pData) = d3d11TextureView->GetDSV(false, true);
            }
            else
            {
                *static_cast<ID3D11DepthStencilView**>(pData) = nullptr;
            }

            return;
        }

        RenderWindow::GetCustomAttribute(name, pData);
    }

    void D3D11HeadlessRenderWindow::SwapBuffers()
    {
        _device.GetImmediateContext()->Flush();
    }
}
