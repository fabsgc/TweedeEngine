#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"
#include "Image/TePixelUtil.h"
#include "Image/TeTexture.h"

namespace te
{
    class GpuResourcePool;
    struct POOLED_RENDER_TEXTURE_DESC;
    struct POOLED_STORAGE_BUFFER_DESC;

    /**	Contains data about a single render texture in the GPU resource pool. */
    struct TE_CORE_EXPORT PooledRenderTexture
    {
        PooledRenderTexture(UINT32 lastUsedFrame)
            : _lastUsedFrame(lastUsedFrame)
        { }

        SPtr<Texture> Tex;
        SPtr<RenderTexture> RenderTex;

    private:
        friend class GpuResourcePool;

        UINT32 _lastUsedFrame = 0;
    };

    /**	Contains data about a single storage buffer in the GPU resource pool. */
    struct TE_CORE_EXPORT PooledStorageBuffer
    {
        PooledStorageBuffer(UINT32 lastUsedFrame)
            : _lastUsedFrame(lastUsedFrame)
        { }

        SPtr<GpuBuffer> Buffer;

    private:
        friend class GpuResourcePool;

        UINT32 _lastUsedFrame = 0;
    };

    /**
     * Contains a pool of textures and buffers meant to accommodate reuse of such resources for the main purpose of using
     * them as write targets on the GPU.
     */
    class TE_CORE_EXPORT GpuResourcePool : public Module<GpuResourcePool>
    {
    public:
        TE_MODULE_STATIC_HEADER_MEMBER(GpuResourcePool)

        /**
         * Attempts to find the unused render texture with the specified parameters in the pool, or creates a new texture
         * otherwise.
         *
         * @param[in]	desc		Descriptor structure that describes what kind of texture to retrieve.
         */
        SPtr<PooledRenderTexture> Get(const POOLED_RENDER_TEXTURE_DESC& desc);

        /**
         * Attempts to find the unused render texture with the specified parameters in the pool, or creates a new texture
         * otherwise. Use this variant of the method if you are already holding a reference to a pooled texture which
         * you want to reuse - this is more efficient than releasing the old texture and calling the other get() variant.
         *
         * @param[in, out]	texture		Existing reference to a pooled texture that you would prefer to reuse. If it
         *								matches the provided descriptor the system will return the unchanged texture,
         *								otherwise it will try to find another unused texture, or allocate a new one. New
         *								value will be output through this parameter.
         * @param[in]		desc		Descriptor structure that describes what kind of texture to retrieve.
         */
        void Get(SPtr<PooledRenderTexture>& texture, const POOLED_RENDER_TEXTURE_DESC& desc);

        /**
         * Attempts to find the unused storage buffer with the specified parameters in the pool, or creates a new buffer
         * otherwise.
         *
         * @param[in]	desc		Descriptor structure that describes what kind of buffer to retrieve.
         */
        SPtr<PooledStorageBuffer> Get(const POOLED_STORAGE_BUFFER_DESC& desc);

        /**
         * Attempts to find the unused storage buffer with the specified parameters in the pool, or creates a new buffer
         * otherwise. Use this variant of the method if you are already holding a reference to a pooled buffer which
         * you want to reuse - this is more efficient than releasing the old buffer and calling the other get() variant.
         *
         * @param[in, out]	buffer		Existing reference to a pooled buffer that you would prefer to reuse. If it
         *								matches the provided descriptor the system will return the unchanged buffer,
         *								otherwise it will try to find another unused buffer, or allocate a new one. New
         *								value will be output through this parameter.
         * @param[in]	desc			Descriptor structure that describes what kind of buffer to retrieve.
         */
        void Get(SPtr<PooledStorageBuffer>& buffer, const POOLED_STORAGE_BUFFER_DESC& desc);

        /** Lets the pool know that another frame has passed. */
        void Update();

        /**
         * Destroys all unreferenced resources with that were last used @p age frames ago. Specify 0 to destroy all
         * unreferenced resources.
         */
        void Prune(UINT32 age);

    private:
        /**
         * Checks does the provided texture match the parameters.
         *
         * @param[in]	texture		Texture to check.
         * @param[in]	desc		Descriptor structure that describes what kind of texture to match.
         * @return					True if the texture matches the descriptor, false otherwise.
         */
        static bool Matches(const SPtr<Texture>& texture, const POOLED_RENDER_TEXTURE_DESC& desc);

        /**
         * Checks does the provided buffer match the parameters.
         *
         * @param[in]	buffer	Buffer to check.
         * @param[in]	desc	Descriptor structure that describes what kind of buffer to match.
         * @return				True if the buffer matches the descriptor, false otherwise.
         */
        static bool Matches(const SPtr<GpuBuffer>& buffer, const POOLED_STORAGE_BUFFER_DESC& desc);

    private:
        Vector<SPtr<PooledRenderTexture>> _textures;
        Vector<SPtr<PooledStorageBuffer>> _buffers;

        UINT32 _currentFrame = 0;
    };

    /** Structure used for creating a new pooled render texture. */
    struct TE_CORE_EXPORT POOLED_RENDER_TEXTURE_DESC
    {
    public:
        POOLED_RENDER_TEXTURE_DESC() {}

        /**
         * Creates a descriptor for a two dimensional render texture.
         *
         * @param[in]	format		Pixel format used by the texture surface.
         * @param[in]	width		Width of the render texture, in pixels.
         * @param[in]	height		Height of the render texture, in pixels.
         * @param[in]	usage		Usage flags that control in which way is the texture going to be used.
         * @param[in]	samples		If higher than 1, texture containing multiple samples per pixel is created.
         * @param[in]	hwGamma		Should the written pixels be gamma corrected.
         * @param[in]	arraySize	Number of textures in a texture array. Specify 1 for no array.
         * @param[in]	mipCount	Number of mip levels, excluding the root mip level.
         * @return					Descriptor that is accepted by RenderTexturePool.
         */
        static POOLED_RENDER_TEXTURE_DESC Create2D(PixelFormat format, UINT32 width, UINT32 height,
            INT32 usage = TU_STATIC, UINT32 samples = 0, bool hwGamma = false, UINT32 arraySize = 1, UINT32 mipCount = 0);

        /**
         * Creates a descriptor for a three dimensional render texture.
         *
         * @param[in]	format		Pixel format used by the texture surface.
         * @param[in]	width		Width of the render texture, in pixels.
         * @param[in]	height		Height of the render texture, in pixels.
         * @param[in]	depth		Depth of the render texture, in pixels.
         * @param[in]	usage		Usage flags that control in which way is the texture going to be used.
         * @return					Descriptor that is accepted by RenderTexturePool.
         */
        static POOLED_RENDER_TEXTURE_DESC Create3D(PixelFormat format, UINT32 width, UINT32 height, UINT32 depth,
            INT32 usage = TU_STATIC);

        /**
         * Creates a descriptor for a cube render texture.
         *
         * @param[in]	format		Pixel format used by the texture surface.
         * @param[in]	width		Width of the render texture, in pixels.
         * @param[in]	height		Height of the render texture, in pixels.
         * @param[in]	usage		Usage flags that control in which way is the texture going to be used.
         * @param[in]	arraySize	Number of textures in a texture array. Specify 1 for no array.
         * @return					Descriptor that is accepted by RenderTexturePool.
         */
        static POOLED_RENDER_TEXTURE_DESC CreateCube(PixelFormat format, UINT32 width, UINT32 height,
            INT32 usage = TU_STATIC, UINT32 arraySize = 1);

    private:
        friend class GpuResourcePool;

        UINT32 Width = 0;
        UINT32 Height = 0;
        UINT32 Depth = 0;
        UINT32 NumSamples = 0;
        PixelFormat Format;
        TextureUsage Flag;
        TextureType Type;
        bool HwGamma = false;
        UINT32 ArraySize = 0;
        UINT32 NumMipLevels = 0;
    };

    /** Structure used for describing a pooled storage buffer. */
    struct TE_CORE_EXPORT POOLED_STORAGE_BUFFER_DESC
    {
    public:
        POOLED_STORAGE_BUFFER_DESC() {}

        /**
         * Creates a descriptor for a storage buffer containing primitive data types.
         *
         * @param[in]	format		Format of individual buffer entries.
         * @param[in]	numElements	Number of elements in the buffer.
         * @param[in]	usage		Usage flag hinting the driver how is buffer going to be used.
         */
        static POOLED_STORAGE_BUFFER_DESC CreateStandard(GpuBufferFormat format, UINT32 numElements,
            GpuBufferUsage usage = GBU_LOADSTORE);

        /**
         * Creates a descriptor for a storage buffer containing structures.
         *
         * @param[in]	elementSize		Size of a single structure in the buffer.
         * @param[in]	numElements		Number of elements in the buffer.
         * @param[in]	usage		Usage flag hinting the driver how is buffer going to be used.
         */
        static POOLED_STORAGE_BUFFER_DESC CreateStructured(UINT32 elementSize, UINT32 numElements,
            GpuBufferUsage usage = GBU_LOADSTORE);

    private:
        friend class GpuResourcePool;

        GpuBufferType Type;
        GpuBufferFormat Format;
        GpuBufferUsage Usage;
        UINT32 NumElements = 0;
        UINT32 ElementSize = 0;
    };

    /**	Provides easy access to the GpuResourcePool. */
    TE_CORE_EXPORT GpuResourcePool& gGpuResourcePool();
}
