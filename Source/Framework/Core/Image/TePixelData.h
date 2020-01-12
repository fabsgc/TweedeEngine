#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
	/** Pixel formats usable by images, textures and render surfaces. */
	enum PixelFormat
	{
		/** Unknown pixel format. */
		PF_UNKNOWN= 0,
		/** 8-bit 1-channel pixel format, unsigned normalized. */
		PF_R8	= 1,
		/** 8-bit 2-channel pixel format, unsigned normalized. */
		PF_RG8	= 2,
		/** 8-bit 3-channel pixel format, unsigned normalized. */
		PF_RGB8	= 3,
		/** 8-bit 3-channel pixel format, unsigned normalized. */
		PF_BGR8	= 4,
		/** 8-bit 4-channel pixel format, unsigned normalized. */
		PF_BGRA8= 7,
		/** 8-bit 4-channel pixel format, unsigned normalized. */
		PF_RGBA8= 8,
		/** DXT1/BC1 format containing opaque RGB or 1-bit alpha RGB. 4 bits per pixel. */
		PF_BC1	= 13,
		/** DXT3/BC2 format containing RGB with premultiplied alpha. 4 bits per pixel. */
		PF_BC1a	= 14,
		/** DXT3/BC2 format containing RGB with explicit alpha. 8 bits per pixel. */
		PF_BC2	= 15,
		/** DXT5/BC2 format containing RGB with explicit alpha. 8 bits per pixel. Better alpha gradients than BC2. */
		PF_BC3	= 16,
		/** One channel compressed format. 4 bits per pixel. */
		PF_BC4	= 17,
		/** Two channel compressed format. 8 bits per pixel. */
		PF_BC5	= 18,
		/** Format storing RGB in half (16-bit) floating point format usable for HDR. 8 bits per pixel. */
		PF_BC6H	= 19,
		/**
		 * Format storing RGB with optional alpha channel. Similar to BC1/BC2/BC3 formats but with higher quality and
		 * higher decompress overhead. 8 bits per pixel.
		 */
		PF_BC7	= 20,
		/** 16-bit 1-channel pixel format, signed float. */
		PF_R16F	= 21,
		/** 16-bit 2-channel pixel format, signed float. */
		PF_RG16F= 22,
		/** 16-bit 4-channel pixel format, signed float. */
		PF_RGBA16F= 24,
		/** 32-bit 1-channel pixel format, signed float. */
		PF_R32F	= 25,
		/** 32-bit 2-channel pixel format, signed float. */
		PF_RG32F = 26,
		/** 32-bit 3-channel pixel format, signed float. */
		PF_RGB32F = 27,
		/** 32-bit 4-channel pixel format, signed float. */
		PF_RGBA32F = 28,
		/** Depth stencil format, 32bit depth, 8bit stencil + 24 unused. Depth stored as signed float. */
		PF_D32_S8X24 = 29,
		/** Depth stencil fomrat, 24bit depth + 8bit stencil. Depth stored as unsigned normalized. */
		PF_D24S8 = 30,
		/** Depth format, 32bits. Signed float. */
		PF_D32	= 31,
		/** Depth format, 16bits. Unsigned normalized. */
		PF_D16	= 32,
		/** Packed unsigned float format, 11 bits for red, 11 bits for green, 10 bits for blue. */
		PF_RG11B10F = 33,
		/**
		 * Packed unsigned normalized format, 10 bits for red, 10 bits for green, 10 bits for blue, and two bits for alpha.
		 */
		PF_RGB10A2 = 34,
		/** 8-bit 1-channel pixel format, signed integer. */
		PF_R8I	= 35,
		/** 8-bit 2-channel pixel format, signed integer. */
		PF_RG8I	= 36,
		/** 8-bit 4-channel pixel format, signed integer. */
		PF_RGBA8I = 37,
		/** 8-bit 1-channel pixel format, unsigned integer. */
		PF_R8U	= 38,
		/** 8-bit 2-channel pixel format, unsigned integer. */
		PF_RG8U	= 39,
		/** 8-bit 4-channel pixel format, unsigned integer. */
		PF_RGBA8U = 40,
		/** 8-bit 1-channel pixel format, signed normalized. */
		PF_R8S	= 41,
		/** 8-bit 2-channel pixel format, signed normalized. */
		PF_RG8S	= 42,
		/** 8-bit 4-channel pixel format, signed normalized. */
		PF_RGBA8S = 43,
		/** 16-bit 1-channel pixel format, signed integer. */
		PF_R16I	= 44,
		/** 16-bit 2-channel pixel format, signed integer. */
		PF_RG16I = 45,
		/** 16-bit 4-channel pixel format, signed integer. */
		PF_RGBA16I = 46,
		/** 16-bit 1-channel pixel format, unsigned integer. */
		PF_R16U	= 47,
		/** 16-bit 2-channel pixel format, unsigned integer. */
		PF_RG16U = 48,
		/** 16-bit 4-channel pixel format, unsigned integer. */
		PF_RGBA16U= 49,
		/** 32-bit 1-channel pixel format, signed integer. */
		PF_R32I	= 50,
		/** 32-bit 2-channel pixel format, signed integer. */
		PF_RG32I = 51,
		/** 32-bit 3-channel pixel format, signed integer. */
		PF_RGB32I = 52,
		/** 32-bit 4-channel pixel format, signed integer. */
		PF_RGBA32I = 53,
		/** 32-bit 1-channel pixel format, unsigned integer. */
		PF_R32U	= 54,
		/** 32-bit 2-channel pixel format, unsigned integer. */
		PF_RG32U = 55,
		/** 32-bit 3-channel pixel format, unsigned integer. */
		PF_RGB32U = 56,
		/** 32-bit 4-channel pixel format, unsigned integer. */
		PF_RGBA32U = 57,
		/** 16-bit 1-channel pixel format, signed normalized. */
		PF_R16S	= 58,
		/** 16-bit 2-channel pixel format, signed normalized. */
		PF_RG16S= 59,
		/** 16-bit 4-channel pixel format, signed normalized. */
		PF_RGBA16S = 60,
		/** 16-bit 1-channel pixel format, unsigned normalized. */
		PF_R16	= 61,
		/** 16-bit 2-channel pixel format, unsigned normalized. */
		PF_RG16	= 62,
		/** 16-bit 3-channel pixel format, unsigned normalized. */
		PF_RGB16 = 63,
		/** 16-bit 4-channel pixel format, unsigned normalized. */
		PF_RGBA16 = 64,
		/** Number of pixel formats currently defined. */
		PF_COUNT
	};
}
