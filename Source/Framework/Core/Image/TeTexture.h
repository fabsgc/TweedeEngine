#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"
#include "CoreUtility/TeCoreObject.h"
#include "TePixelData.h"

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

    protected:
        friend class TextureManager;

        Texture(const TEXTURE_DESC& desc);
        Texture(const TEXTURE_DESC& desc, const SPtr<PixelData>& pixelData);

    protected:
        TextureProperties _properties;
        mutable SPtr<PixelData> _initData;
    };
}