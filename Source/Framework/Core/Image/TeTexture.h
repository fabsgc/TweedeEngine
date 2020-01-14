#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"
#include "CoreUtility/TeCoreObject.h"
#include "TePixelData.h"
#include "Math/TeVector3I.h"
#include "RenderAPI/TeCommonTypes.h"
#include "RenderAPI/TeTextureView.h"

namespace te
{
	enum TextureUsage
	{
		/** A regular texture that is not often or ever updated from the CPU. */
		TU_STATIC = GBU_STATIC,
		/** A regular texture that is often updated by the CPU. */
		TU_DYNAMIC = GBU_DYNAMIC,
		/** Texture that can be rendered to by the GPU. */
		TU_RENDERTARGET	= 0x200,
		/** Texture used as a depth/stencil buffer by the GPU. */
		TU_DEPTHSTENCIL= 0x400,
		/** Texture that allows load/store operations from the GPU program. */
		TU_LOADSTORE = 0x800,
		/** All mesh data will also be cached in CPU memory, making it available for fast read access from the CPU. */
		TU_CPUCACHED = 0x1000,
		/** Allows the CPU to directly read the texture data buffers from the GPU. */
		TU_CPUREADABLE = 0x2000,
		/** Allows you to retrieve views of the texture using a format different from one specified on creation. */
		TU_MUTABLEFORMAT = 0x4000,
		/** Default (most common) texture usage. */
		TU_DEFAULT = TU_STATIC
	};

	/**	Texture mipmap options. */
	enum TextureMipmap
	{
		MIP_UNLIMITED = 0x7FFFFFFF /**< Create all mip maps down to 1x1. */
	};

	/** Descriptor structure used for initialization of a Texture. */
	struct TEXTURE_DESC
	{
		/** Type of the texture. */
		TextureType Type = TEX_TYPE_2D;

		/** Format of pixels in the texture. */
		PixelFormat Format = PF_RGBA8;

		/** Width of the texture in pixels. */
		UINT32 Width = 1;

		/** Height of the texture in pixels. */
		UINT32 Height = 1;

		/** Depth of the texture in pixels (Must be 1 for 2D textures). */
		UINT32 Depth = 1;

		/** Number of mip-maps the texture has. This number excludes the full resolution map. */
		UINT32 NumMips = 0;

		/** Describes how the caller plans on using the texture in the pipeline. */
		INT32 Usage = TU_DEFAULT;

		/**
		 * If true the texture data is assumed to have been gamma corrected and will be converted back to linear space when
		 * sampled on GPU.
		 */
		bool HwGamma = false;

		/** Number of samples per pixel. Set to 1 or 0 to use the default of a single sample per pixel. */
		UINT32 NumSamples = 0;

		/** Number of texture slices to create if creating a texture array. Ignored for 3D textures. */
		UINT32 NumArraySlices = 1;
	};

    /** Structure used for specifying information about a texture copy operation. */
    struct TEXTURE_COPY_DESC
    {
        /**
         * Face from which to copy. This can be an entry in an array of textures, or a single face of a cube map. If cubemap
         * array, then each array entry takes up six faces.
         */
        UINT32 SrcFace = 0;

        /** Mip level from which to copy. */
        UINT32 SrcMip = 0;

        /** Pixel volume from which to copy from. This defaults to all pixels of the face. */
        PixelVolume SrcVolume = PixelVolume(0, 0, 0, 0, 0, 0);

        /**
         * Face to which to copy. This can be an entry in an array of textures, or a single face of a cube map. If cubemap
         * array, then each array entry takes up six faces.
         */
        UINT32 DstFace = 0;

        /** Mip level to which to copy. */
        UINT32 DstMip = 0;

        /**
         * Coordinates to write the source pixels to. The destination texture must have enough pixels to fit the entire
         * source volume.
         */
        Vector3I DstPosition;

        TE_CORE_EXPORT static TEXTURE_COPY_DESC DEFAULT;
    };

    /** Properties of a Texture. Shared between sim and core thread versions of a Texture. */
    class TE_CORE_EXPORT TextureProperties
    {
    public:
        TextureProperties() = default;
        TextureProperties(const TEXTURE_DESC& desc);

        /**	Gets the type of texture. */
        TextureType GetTextureType() const { return _desc.Type; }

        /**
         * Gets the number of mipmaps to be used for this texture. This number excludes the top level map (which is always
         * assumed to be present).
         */
        UINT32 GetNumMipmaps() const { return _desc.NumMips; }

        /**
         * Determines does the texture contain gamma corrected data. If true then the GPU will automatically convert the
         * pixels to linear space before reading from the texture, and convert them to gamma space when writing to the
         * texture.
         */
        bool IsHardwareGammaEnabled() const { return _desc.HwGamma; }

        /**	Gets the number of samples used for multisampling (0 or 1 if multisampling is not used). */
        UINT32 GetNumSamples() const { return _desc.NumSamples; }

        /**	Returns the height of the texture.  */
        UINT32 GetHeight() const { return _desc.Height; }

        /**	Returns the width of the texture. */
        UINT32 GetWidth() const { return _desc.Width; }

        /**	Returns the depth of the texture (only applicable for 3D textures). */
        UINT32 GetDepth() const { return _desc.Depth; }

        /**	Returns a value that signals the engine in what way is the texture expected to be used. */
        int GetUsage() const { return _desc.Usage; }

        /**	Returns the pixel format for the texture surface. */
        PixelFormat GetFormat() const { return _desc.Format; }

        /**	Returns true if the texture has an alpha layer. */
        bool HasAlpha() const;

        /**
         * Returns the number of faces this texture has. This includes array slices (if texture is an array texture),
         * as well as cube-map faces.
         */
        UINT32 GetNumFaces() const;

        /** Returns the number of array slices of the texture (if the texture is an array texture). */
        UINT32 GetNumArraySlices() const { return _desc.NumArraySlices; }

        /**
         * Allocates a buffer that exactly matches the format of the texture described by these properties, for the provided
         * face and mip level. This is a helper function, primarily meant for creating buffers when reading from, or writing
         * to a texture.
         *
         * @note	Thread safe.
         */
        SPtr<PixelData> AllocBuffer(UINT32 face, UINT32 mipLevel) const;

    protected:
        friend class Texture;

        /**
         * Maps a sub-resource index to an exact face and mip level. Sub-resource indexes are used when reading or writing
         * to the resource.
         */
        void MapFromSubresourceIdx(UINT32 subresourceIdx, UINT32& face, UINT32& mip) const;

        /**
         * Map a face and a mip level to a sub-resource index you can use for updating or reading a specific sub-resource.
         */
        UINT32 MapToSubresourceIdx(UINT32 face, UINT32 mip) const;

        TEXTURE_DESC _desc;
    };

	class TE_CORE_EXPORT Texture : public Resource
	{
	public:
		Texture() = default;
		virtual ~Texture() = default;

        /** @copydoc CoreObject::Initialize */
        void Initialize() override;

        /**
		 * Locks the buffer for reading or writing.
		 *
		 * @param[in]	options 	Options for controlling what you may do with the locked data.
		 * @param[in]	mipLevel	(optional) Mipmap level to lock.
		 * @param[in]	face		(optional) Texture face to lock.
		 * @param[in]	deviceIdx	Index of the device whose memory to map. If the buffer doesn't exist on this device,
		 *							the method returns null.
		 * @param[in]	queueIdx	Device queue to perform the read/write operations on. See @ref queuesDoc.
		 * 			
		 * @note	
		 * If you are just reading or writing one block of data use readData()/writeData() methods as they can be much faster
		 * in certain situations.
		 */
		PixelData Lock(GpuLockOptions options, UINT32 mipLevel = 0, UINT32 face = 0, UINT32 deviceIdx = 0, UINT32 queueIdx = 0);

		/**
		 * Unlocks a previously locked buffer. After the buffer is unlocked, any data returned by lock becomes invalid.
		 *
		 * @see	lock()
		 */
		void Unlock();

        /**
         * Copies the contents a subresource in this texture to another texture. Texture format and size of the subresource
         * must match.
         *
         * You are allowed to copy from a multisampled to non-multisampled surface, which will resolve the multisampled
         * surface before copying.
         *
         * @param[in]	target				Texture that contains the destination subresource.
         * @param[in]	desc				Structure used for customizing the copy operation.
         */
        void Copy(const SPtr<Texture>& target, const TEXTURE_COPY_DESC& desc = TEXTURE_COPY_DESC::DEFAULT);

        /**
         * Sets all the pixels of the specified face and mip level to the provided value.
         *
         * @param[in]	value			Color to clear the pixels to.
         * @param[in]	mipLevel		Mip level to clear.
         * @param[in]	face			Face (array index or cubemap face) to clear.
         * @param[in]	queueIdx		Device queue to perform the write operation on. See @ref queuesDoc.
         */
        void Clear(const Color& value, UINT32 mipLevel = 0, UINT32 face = 0, UINT32 queueIdx = 0);

        /**
         * Reads data from the texture buffer into the provided buffer.
         *
         * @param[out]	dest		Previously allocated buffer to read data into.
         * @param[in]	mipLevel	(optional) Mipmap level to read from.
         * @param[in]	face		(optional) Texture face to read from.
         * @param[in]	deviceIdx	Index of the device whose memory to read. If the buffer doesn't exist on this device,
         *							no data will be read.
         * @param[in]	queueIdx	Device queue to perform the read operation on. See @ref queuesDoc.
         */
        void ReadData(PixelData& dest, UINT32 mipLevel = 0, UINT32 face = 0, UINT32 deviceIdx = 0, UINT32 queueIdx = 0);

        /**
         * Writes data from the provided buffer into the texture buffer.
         *
         * @param[in]	src					Buffer to retrieve the data from.
         * @param[in]	mipLevel			(optional) Mipmap level to write into.
         * @param[in]	face				(optional) Texture face to write into.
         * @param[in]	discardWholeBuffer	(optional) If true any existing texture data will be discard. This can improve
         *									performance of the write operation.
         * @param[in]	queueIdx			Device queue to perform the write operation on. See @ref queuesDoc.
         */
        void WriteData(const PixelData& src, UINT32 mipLevel = 0, UINT32 face = 0, bool discardWholeBuffer = false, UINT32 queueIdx = 0);

        /**	Returns properties that contain information about the texture. */
        const TextureProperties& GetProperties() const { return _properties; }

        /** Calculates the size of the texture, in bytes. */
        UINT32 CalculateSize() const;

        /** Creates a new empty texture. */
        static HTexture Create(const TEXTURE_DESC& desc);

        /**
         * Creates a new 2D or 3D texture initialized using the provided pixel data. Texture will not have any mipmaps.
         *
         * @param[in]	pixelData			Data to initialize the texture width.
         * @param[in]	usage				Describes planned texture use.
         * @param[in]	hwGammaCorrection	If true the texture data is assumed to have been gamma corrected and will be
         *									converted back to linear space when sampled on GPU.
         */
        static HTexture Create(const SPtr<PixelData>& pixelData, int usage = TU_DEFAULT, bool hwGammaCorrection = false);

        /** Same as create() excepts it creates a pointer to the texture instead of a texture handle. */
        static SPtr<Texture> _createPtr(const TEXTURE_DESC& desc);

        /** Same as create() excepts it creates a pointer to the texture instead of a texture handle. */
        static SPtr<Texture> _createPtr(const SPtr<PixelData>& pixelData, int usage = TU_DEFAULT, bool hwGammaCorrection = false);

        /**
         * Requests a texture view for the specified mip and array ranges. Returns an existing view of one for the specified
         * ranges already exists, otherwise creates a new one. You must release all views by calling releaseView() when done.
         */
        SPtr<TextureView> RequestView(UINT32 mostDetailMip, UINT32 numMips, UINT32 firstArraySlice, UINT32 numArraySlices, GpuViewUsage usage);

        /** Returns a plain white texture. */
        static SPtr<Texture> WHITE;

        /** Returns a plain black texture. */
        static SPtr<Texture> BLACK;

        /** Returns a plain normal map texture with normal pointing up (in Y direction). */
        static SPtr<Texture> NORMAL;

    protected:
        friend class TextureManager;

        Texture(const TEXTURE_DESC& desc);
        Texture(const TEXTURE_DESC& desc, const SPtr<PixelData>& pixelData);

        /** @copydoc Lock */
		virtual PixelData LockImpl(GpuLockOptions options, UINT32 mipLevel = 0, UINT32 face = 0, UINT32 deviceIdx = 0, UINT32 queueIdx = 0) = 0;

		/** @copydoc unlock */
		virtual void UnlockImpl() = 0;

		/** @copydoc copy */
		virtual void CopyImpl(const SPtr<Texture>& target, const TEXTURE_COPY_DESC& desc) = 0;

		/** @copydoc readData */
		virtual void ReadDataImpl(PixelData& dest, UINT32 mipLevel = 0, UINT32 face = 0, UINT32 deviceIdx = 0, UINT32 queueIdx = 0) = 0;

		/** @copydoc writeData */
		virtual void WriteDataImpl(const PixelData& src, UINT32 mipLevel = 0, UINT32 face = 0, bool discardWholeBuffer = false, UINT32 queueIdx = 0) = 0;

		/** @copydoc clear */
		virtual void ClearImpl(const Color& value, UINT32 mipLevel = 0, UINT32 face = 0, UINT32 queueIdx = 0);

        /**	Creates a view of a specific subresource in a texture. */
        virtual SPtr<TextureView> CreateView(const TEXTURE_VIEW_DESC& desc);

        /** Releases all internal texture view references. */
        void ClearBufferViews();

    protected:
        UnorderedMap<TEXTURE_VIEW_DESC, SPtr<TextureView>, TextureView::HashFunction, TextureView::EqualFunction> _textureViews;

        TextureProperties _properties;
        mutable SPtr<PixelData> _initData;
    };
}
