//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "Image/TeTexture.h"

namespace te
{
	/**	DirectX 11 implementation of a texture. */
    class D3D11Texture : public Texture
    {
    public:
        ~D3D11Texture();

        /**	Returns internal DX11 texture resource object. */
        ID3D11Resource* GetDX11Resource() const { return _tex; }

        /**	Returns shader resource view associated with the texture. */
        ID3D11ShaderResourceView* GetSRV() const;

        /** Returns DXGI pixel format that was used to create the texture. */
        DXGI_FORMAT GetDXGIFormat() const { return _DXGIFormat; }

        /** Returns DXGI pixel used for reading the texture as a shader resource or writing as a render target. */
        DXGI_FORMAT GetColorFormat() const { return _DXGIColorFormat; }

        /** Returns DXGI pixel used for writing to a depth stencil texture. */
        DXGI_FORMAT GetDepthStencilFormat() const { return _DXGIDepthStencilFormat; }

    protected:
        friend class D3D11TextureManager;

        D3D11Texture(const TEXTURE_DESC& desc, const SPtr<PixelData>& initialData);

        /** @copydoc CoreObject::Initialize() */
        void Initialize() override;

        /**	Creates a blank DX11 1D texture object. */
        void Create1DTex();

        /**	Creates a blank DX11 2D texture object. */
        void Create2DTex();

        /**	Creates a blank DX11 3D texture object. */
        void Create3DTex();

        /**
         * Creates a staging buffer that is used as a temporary buffer for read operations on textures that do not support
         * direct reading.
         */
        void CreateStagingBuffer();

    protected:
        ID3D11Texture1D* _1DTex = nullptr;
        ID3D11Texture2D* _2DTex = nullptr;
        ID3D11Texture3D* _3DTex = nullptr;
        ID3D11Resource* _tex = nullptr;

        PixelFormat _internalFormat = PF_UNKNOWN;
        DXGI_FORMAT _DXGIFormat = DXGI_FORMAT_UNKNOWN;
        DXGI_FORMAT _DXGIColorFormat = DXGI_FORMAT_UNKNOWN;
        DXGI_FORMAT _DXGIDepthStencilFormat = DXGI_FORMAT_UNKNOWN;

        ID3D11Resource* _stagingBuffer = nullptr;
        PixelData* _staticBuffer = nullptr;
    };
}
