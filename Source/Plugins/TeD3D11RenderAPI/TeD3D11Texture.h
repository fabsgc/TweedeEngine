#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "TeD3D11TextureView.h"
#include "Image/TeTexture.h"
#include "Threading/TeThreading.h"

namespace te
{
    /** DirectX 11 implementation of a texture. */
    class D3D11Texture : public Texture
    {
    public:
        ~D3D11Texture();

        /** Returns internal DX11 texture resource object. */
        ID3D11Resource* GetDX11Resource() const { return _tex; }

        /** Returns shader resource view associated with the texture. */
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

        /** @copydoc CoreObject::Initialize */
        void Initialize() override;

        /** @copydoc Texture::LockImpl */
        PixelData LockImpl(GpuLockOptions options, UINT32 mipLevel = 0, UINT32 face = 0, UINT32 deviceIdx = 0, UINT32 queueIdx = 0) override;

        /** @copydoc Texture::UnlockImpl */
        void UnlockImpl() override;

        /** @copydoc Texture::CopyImpl */
        void CopyImpl(const SPtr<Texture>& target, const TEXTURE_COPY_DESC& desc) override;

        /** @copydoc Texture::ReadData */
        void ReadDataImpl(PixelData& dest, UINT32 mipLevel = 0, UINT32 face = 0, UINT32 deviceIdx = 0, UINT32 queueIdx = 0) override;

        /** @copydoc Texture::WriteData */
        void WriteDataImpl(const PixelData& src, UINT32 mipLevel = 0, UINT32 face = 0, bool discardWholeBuffer = false, UINT32 queueIdx = 0) override;

        /** Creates a blank DX11 1D texture object. */
        void Create1DTex();

        /** Creates a blank DX11 2D texture object. */
        void Create2DTex();

        /** Creates a blank DX11 3D texture object. */
        void Create3DTex();

        /**
         * Creates a staging buffer that is used as a temporary buffer for read operations on textures that do not support
         * direct reading.
         */
        void CreateStagingBuffer();

        /**
         * Maps the specified texture surface for reading/writing.
         *
         * @param[in]	res			Texture resource to map.
         * @param[in]	flags		Mapping flags that let the API know what are we planning to do with mapped memory.
         * @param[in]	mipLevel	Mip level to map (0 being the base level).
         * @param[in]	face		Texture face to map, in case texture has more than one.
         * @param[out]	rowPitch	Output size of a single row in bytes.
         * @param[out]	slicePitch	Output size of a single slice in bytes (relevant only for 3D textures).
         * @return					Pointer to the mapped area of memory.
         *
         * @note
         * Non-staging textures must be dynamic in order to be mapped directly and only for writing. No restrictions are
         * made on staging textures.
         */
        void* Map(ID3D11Resource* res, D3D11_MAP flags, UINT32 mipLevel, UINT32 face, UINT32& rowPitch, UINT32& slicePitch);

        /** Unmaps a previously mapped texture. */
        void Unmap(ID3D11Resource* res);

        /**
         * Copies texture data into a staging buffer and maps the staging buffer. Will create a staging buffer if one
         * doesn't already exist (potentially wasting a lot of memory).
         *
         * @param[in]	flags		Mapping flags that let the API know what are we planning to do with mapped memory.
         * @param[in]	mipLevel	Mip level to map (0 being the base level).
         * @param[in]	face		Texture face to map, in case texture has more than one.
         * @param[out]	rowPitch	Output size of a single row in bytes.
         * @param[out]	slicePitch	Output size of a single slice in bytes (relevant only for 3D textures).
         * @return					Pointer to the mapped area of memory.
         */
        void* Mapstagingbuffer(D3D11_MAP flags, UINT32 mipLevel, UINT32 face, UINT32& rowPitch, UINT32& slicePitch);

        /** Unmaps a previously mapped staging buffer. */
        void Unmapstagingbuffer();

        /**
         * Maps a static buffer, for writing only. Returned pointer points to temporary CPU memory that will be copied to
         * the mapped resource on "unmap" call.
         *
         * @param[in]	lock		Area of the texture to lock.
         * @param[in]	mipLevel	Mip level to map (0 being the base level).
         * @param[in]	face		Texture face to map, in case texture has more than one.
         */
        void* Mapstaticbuffer(PixelData lock, UINT32 mipLevel, UINT32 face);

        /** Unmaps a previously mapped static buffer and flushes its data to the actual GPU buffer. */
        void Unmapstaticbuffer();

        /** Creates an empty and uninitialized texture view object. */
        SPtr<TextureView> CreateView(const TEXTURE_VIEW_DESC& desc) override;

    protected:
        ID3D11Texture1D* _1DTex = nullptr;
        ID3D11Texture2D* _2DTex = nullptr;
        ID3D11Texture3D* _3DTex = nullptr;
        ID3D11Resource* _tex = nullptr;

        SPtr<D3D11TextureView> _shaderResourceView;

        PixelFormat _internalFormat = PF_UNKNOWN;
        DXGI_FORMAT _DXGIFormat = DXGI_FORMAT_UNKNOWN;
        DXGI_FORMAT _DXGIColorFormat = DXGI_FORMAT_UNKNOWN;
        DXGI_FORMAT _DXGIDepthStencilFormat = DXGI_FORMAT_UNKNOWN;

        ID3D11Resource* _stagingBuffer = nullptr;
        PixelData* _staticBuffer = nullptr;
        UINT32 _lockedSubresourceIdx = static_cast<UINT32>(-1);
        bool _lockedForReading = false;

        static RecursiveMutex _deviceMutex;
    };
}
