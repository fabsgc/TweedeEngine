#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "RenderAPI/TeTextureView.h"

namespace te
{
    /**	DirectX implementation of a texture resource view. */
    class D3D11TextureView : public TextureView
    {
    public:
        virtual ~D3D11TextureView();

        /**
         * Returns a shader resource view. Caller must take care this texture view actually contains a shader resource view,
         * otherwise it returns null.
         */
        ID3D11ShaderResourceView* GetSRV() const { return _SRV; }

        /**
         * Returns a render target view. Caller must take care this texture view actually contains a render target view,
         * otherwise it returns null.
         */
        ID3D11RenderTargetView*	GetRTV() const { return _RTV; }

        /**
         * Returns a unordered access view. Caller must take care this texture view actually contains a unordered access
         * view, otherwise it returns null.
         */
        ID3D11UnorderedAccessView* GetUAV() const { return _UAV; }

        /**
         * Returns a depth stencil view. Caller must take care this texture view actually contains a depth stencil view,
         * otherwise it returns null.
         *
         * @param[in]	readOnlyDepth	Should the view only support read operations for the depth portion of the
         *								depth/stencil buffer(allows the bound texture to be also used as a shader resource
         *								view while bound as a depth stencil target).
         * @param[in]	readOnlyStencil	Should the view only support read operations for the stencil portion of the
         *								depth/stencil buffer(allows the bound texture to be also used as a shader resource
         *								view while bound as a depth stencil target).
         */
        ID3D11DepthStencilView*	GetDSV(bool readOnlyDepth, bool readOnlyStencil) const;

        /** @copydoc TextureView::GetRawData */
        void* GetRawData() override { return (void*)GetSRV(); }

    protected:
        friend class D3D11Texture;

        D3D11TextureView(const D3D11Texture* texture, const TEXTURE_VIEW_DESC& desc);
    private:
        /**
         * Creates a shader resource view that allows the provided surfaces to be bound as normal shader resources.
         *
         * @param[in]	texture			Texture to create the resource view for.
         * @param[in]	mostDetailMip	First mip level to create the resource view for (0 - base level).
         * @param[in]	numMips			Number of mip levels to create the view for.
         * @param[in]	firstArraySlice	First array slice to create the view for. This will be array index for 1D and 2D
         *								array textures, texture slice index for 3D textures, and face index for cube
         *								textures (cube index * 6).
         * @param[in]	numArraySlices	Number of array slices to create the view for. This will be number of array elements
         *								for 1D and 2D array textures, number of slices for 3D textures, and number of cubes
         *								for cube textures.
         */
        ID3D11ShaderResourceView* CreateSRV(const D3D11Texture* texture,
            UINT32 mostDetailMip, UINT32 numMips, UINT32 firstArraySlice, UINT32 numArraySlices);

        /**
         * Creates a shader resource view that allows the provided surfaces to be bound as render targets.
         *
         * @param[in]	texture			Texture to create the resource view for.
         * @param[in]	mipSlice		Mip level to create the resource view for (0 - base level).
         * @param[in]	firstArraySlice	First array slice to create the view for. This will be array index for 1D and 2D
         *								array textures, texture slice index for 3D textures, and face index for cube
         *								textures (cube index * 6).
         * @param[in]	numArraySlices	Number of array slices to create the view for. This will be number of array elements
         *								for 1D and 2D array textures, number of slices for 3D textures, and number of cubes
         *								for cube textures.
         */
        ID3D11RenderTargetView* CreateRTV(const D3D11Texture* texture,
            UINT32 mipSlice, UINT32 firstArraySlice, UINT32 numArraySlices);

        /**
         * Creates a shader resource view that allows the provided surfaces to be bound as unordered access buffers.
         *
         * @param[in]	texture			Texture to create the resource view for.
         * @param[in]	mipSlice		Mip level to create the resource view for (0 - base level).
         * @param[in]	firstArraySlice	First array slice to create the view for. This will be array index for 1D and 2D
         *								array textures, texture slice index for 3D textures, and face index for cube
         *								textures (cube index * 6).
         * @param[in]	numArraySlices	Number of array slices to create the view for. This will be number of array elements
         *								for 1D and 2D array textures, number of slices for 3D textures, and number of cubes
         *								for cube textures.
         */
        ID3D11UnorderedAccessView* CreateUAV(const D3D11Texture* texture,
            UINT32 mipSlice, UINT32 firstArraySlice, UINT32 numArraySlices);

        /**
         * Creates a shader resource view that allows the provided surfaces to be bound as depth stencil buffers.
         *
         * @param[in]	texture			Texture to create the resource view for.
         * @param[in]	mipSlice		Mip level to create the resource view for (0 - base level).
         * @param[in]	firstArraySlice	First array slice to create the view for. This will be array index for 1D and 2D
         *								array textures, texture slice index for 3D textures, and face index for cube
         *								textures (cube index * 6).
         * @param[in]	numArraySlices	Number of array slices to create the view for. This will be number of array elements
         *								for 1D and 2D array textures, number of slices for 3D textures, and number of cubes
         *								for cube textures.
         * @param[in]	readOnlyDepth	Should the view only support read operations for the depth portion of the
         *								depth/stencil buffer(allows the bound texture to be also used as a shader resource
         *								view while bound as a depth stencil target).
         * @param[in]	readOnlyStencil	Should the view only support read operations for the stencil portion of the
         *								depth/stencil buffer(allows the bound texture to be also used as a shader resource
         *								view while bound as a depth stencil target).
         */
        ID3D11DepthStencilView* CreateDSV(const D3D11Texture* texture, UINT32 mipSlice, UINT32 firstArraySlice,
            UINT32 numArraySlices, bool readOnlyDepth, bool readOnlyStencil);

        ID3D11ShaderResourceView* _SRV = nullptr;
        ID3D11RenderTargetView* _RTV = nullptr;
        ID3D11UnorderedAccessView* _UAV = nullptr;
        ID3D11DepthStencilView*	_WDepthWStencilView = nullptr;
        ID3D11DepthStencilView*	_RODepthWStencilView = nullptr;
        ID3D11DepthStencilView*	_RODepthROStencilView = nullptr;
        ID3D11DepthStencilView*	_WDepthROStencilView = nullptr;
    };
}
