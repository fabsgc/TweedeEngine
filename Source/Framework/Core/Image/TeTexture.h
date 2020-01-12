#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"
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

	class TE_CORE_EXPORT Texture : public Resource
	{
	public:
		Texture() = default;
		virtual ~Texture() = default;
    };
}