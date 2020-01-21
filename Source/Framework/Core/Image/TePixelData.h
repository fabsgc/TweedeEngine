#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeGpuResourceData.h"
#include "Image/TePixelVolume.h"

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

	/**	Flags defining some properties of pixel formats. */
	enum PixelFormatFlags {
		/** This format has an alpha channel. */
		PFF_HASALPHA = 0x1,
		/**
		 * This format is compressed. This invalidates the values in elemBytes, elemBits and the bit counts as these might
		 * not be fixed in a compressed format.
		 */
		PFF_COMPRESSED = 0x2,
		/** This is a floating point format. */
		PFF_FLOAT = 0x4,
		/** This is a depth format (for depth textures). */
		PFF_DEPTH = 0x8,
		/** This format stores data internally as integers. */
		PFF_INTEGER = 0x10,
		/** Format contains signed data. Absence of this flag implies unsigned data. */
		PFF_SIGNED = 0x20,
		/** Format contains normalized data. This will be [0, 1] for unsigned, and [-1,1] for signed formats. */
		PFF_NORMALIZED = 0x40
	};

	/** Determines how are texture pixels filtered during sampling. */
	enum TextureFilter
	{
		/** Pixel nearest to the sampled location is chosen. */
		TF_NEAREST,
		/** Four pixels nearest to the sampled location are interpolated to yield the sampled color. */
		TF_BILINEAR
	};

	/**	Types used for individual components of a pixel. */
	enum PixelComponentType
	{
		PCT_BYTE = 0, /**< 8-bit integer per component */
		PCT_SHORT = 1, /**< 16-bit integer per component. */
		PCT_INT = 2, /**< 32-bit integer per component. */
		PCT_FLOAT16 = 3, /**< 16 bit float per component */
		PCT_FLOAT32 = 4, /**< 32 bit float per component */
		PCT_PACKED_R11G11B10 = 5, /**< 11 bits for first two components, 10 for third component. */
		PCT_PACKED_R10G10B10A2 = 6, /**< 10 bits for first three components, 2 bits for last component */
		PCT_COUNT    /**< Number of pixel types */
	};

	/** A list of cubemap faces. */
	enum CubemapFace
	{
		CF_PositiveX,
		CF_NegativeX,
		CF_PositiveY,
		CF_NegativeY,
		CF_PositiveZ,
		CF_NegativeZ
	};

	/**
	 * A buffer describing a volume (3D), image (2D) or line (1D) of pixels in memory. Pixels are stored as a succession
	 * of "depth" slices, each containing "height" rows of "width" pixels.
	 *
	 * @note
	 * If using the constructor instead of create() you must call GpuResourceData::allocateInternalBuffer or set the buffer
	 * in some other way before reading/writing from this object, as by the default there is no buffer allocated.
	 *
	 * @see		GpuResourceData
	 */
	class TE_CORE_EXPORT PixelData : public GpuResourceData
	{
	public:
		PixelData() = default;

		/**	Return the size (in bytes) of the buffer this image requires. */
		UINT32 GetSize() const;

		/**
		 * Constructs a new object with an internal buffer capable of holding "extents" volume of pixels, where each pixel
		 * is of the specified pixel format. Extent offsets are also stored, but are not used internally.
		 */
		PixelData(const PixelVolume& extents, PixelFormat pixelFormat);

		/**
		 * Constructs a new object with an internal buffer capable of holding volume of pixels described by	provided width,
		 * height and depth, where each pixel is of the specified pixel format.
		 */
		PixelData(UINT32 width, UINT32 height, UINT32 depth, PixelFormat pixelFormat);

		PixelData(const PixelData& copy);
		PixelData& operator=(const PixelData& rhs);

		/**
		 * Returns the number of bytes that offsets one row from another. This can be exact number of bytes required
		 * to hold "width" pixel, but doesn't have to be as some buffers require padding.
		 */
		UINT32 GetRowPitch() const { return _rowPitch; }

		/**
		 * Returns the number of bytes that offsets one depth slice from another. This can be exact number of bytes
		 * required to hold "width * height" pixels, but doesn't have to be as some buffers require padding.
		 */
		UINT32 GetSlicePitch() const { return _slicePitch; }

		/**
		 * Sets the pitch (in bytes) that determines offset between rows of the pixel buffer. Call this before allocating
		 * the buffer.
		 */
		void SetRowPitch(UINT32 rowPitch) { _rowPitch = rowPitch; }

		/**
		 * Sets the pitch (in bytes) that determines offset between depth slices of the pixel buffer. Call this before
		 * allocating the buffer.
		 */
		void SetSlicePitch(UINT32 slicePitch) { _slicePitch = slicePitch; }

		/**
		 * Returns the number of extra bytes in a row (non-zero only if rows are not consecutive (row pitch is larger
		 * than the number of bytes required to hold "width" pixels)).
		 */
		UINT32 GetRowSkip() const;

		/**
		 * Returns the number of extra bytes in a depth slice (non-zero only if slices aren't consecutive (slice pitch is
		 * larger than the number of bytes required to hold "width * height").
		 */
		UINT32 GetSliceSkip() const;

		/** Returns the pixel format used by the internal buffer for storing the pixels. */
		PixelFormat GetFormat() const { return _format; }

		/**	Returns width of the buffer in pixels. */
		UINT32 GetWidth() const { return _extents.GetWidth(); }

		/**	Returns height of the buffer in pixels. */
		UINT32 GetHeight() const { return _extents.GetHeight(); }

		/**	Returns depth of the buffer in pixels. */
		UINT32 GetDepth() const { return _extents.GetDepth(); }

		/**
		 * Returns left-most start of the pixel volume. This value is not used internally in any way. It is just passed
		 * through from the constructor.
		 */
		UINT32 GetLeft() const { return _extents.Left; }

		/**
		 * Returns right-most end of the pixel volume. This value is not used internally in any way. It is just passed
		 * through from the constructor.
		 */
		UINT32 GetRight() const { return _extents.Right; }

		/**
		 * Returns top-most start of the pixel volume. This value is not used internally in any way. It is just passed
		 * through from the constructor.
		 */
		UINT32 GetTop() const { return _extents.Top; }

		/**
		 * Returns bottom-most end of the pixel volume. This value is not used internally in any way. It is just passed
		 * through from the constructor.
		 */
		UINT32 GetBottom() const { return _extents.Bottom; }

		/**
		 * Returns front-most start of the pixel volume. This value is not used internally in any way. It is just passed
		 * through from the constructor.
		 */
		UINT32 GetFront() const { return _extents.Front; }

		/**
		 * Returns back-most end of the pixel volume. This value is not used internally in any way. It is just passed
		 * through from the constructor.
		 */
		UINT32 GetBack() const { return _extents.Back; }

		/** Returns extents of the pixel volume this object is capable of holding. */
		PixelVolume GetExtents() const { return _extents; }

		/**
		 * Return whether this buffer is laid out consecutive in memory (meaning the pitches are equal to the dimensions).
		 */
		bool IsConsecutive() const
		{
			return _slicePitch * GetDepth() == GetConsecutiveSize();
		}

		/** Return the size (in bytes) this image would take if it was laid out consecutive in memory. */
		UINT32 GetConsecutiveSize() const;

		/**
		 * Returns pixel data containing a sub-volume of this object. Returned data will not have its own buffer, but will
		 * instead point to this one. It is up to the caller to ensure this object outlives any sub-volume objects.
		 */
		PixelData GetSubVolume(const PixelVolume& volume) const;

		/**
		 * Samples a color at the specified coordinates using a specific filter.
		 *
		 * @param[in]	coords	Coordinates to sample the color at. They start at top left corner (0, 0), and are in range
		 *						[0, 1].
		 * @param[in]	filter	Filtering mode to use when sampling the color.
		 * @return				Sampled color.
		 */
		Color SampleColorAt(const Vector2& coords, TextureFilter filter = TF_BILINEAR) const;

		/**	Returns pixel color at the specified coordinates. */
		Color GetColorAt(UINT32 x, UINT32 y, UINT32 z = 0) const;

		/**	Sets the pixel color at the specified coordinates. */
		void SetColorAt(const Color& color, UINT32 x, UINT32 y, UINT32 z = 0);

		/**
		 * Converts all the internal data into an array of colors. Array is mapped as such:
		 * arrayIdx = x + y * width + z * width * height.
		 */
		Vector<Color> GetColors() const;

		/**
		 * Initializes the internal buffer with the provided set of colors. The array should be of width * height * depth
		 * size and mapped as such: arrayIdx = x + y * width + z * width * height.
		 */
		void SetColors(const Vector<Color>& colors);

		/**
		 * Initializes the internal buffer with the provided set of colors. The array should be of
		 * width * height * depth size and mapped as such: arrayIdx = x + y * width + z * width * height.
		 */
		void SetColors(Color* colors, UINT32 numElements);

		/** Initializes all the pixels with a single color. */
		void SetColors(const Color& color);

		/**
		 * Interprets pixel data as depth information as retrieved from the GPU's depth buffer. Converts the device specific
		 * depth value to range [0, 1] and returns it.
		 */
		float GetDepthAt(UINT32 x, UINT32 y, UINT32 z = 0) const;

		/**
		 * Converts all the internal data into an array of floats as if each individual pixel is retrieved with
		 * getDepthAt(). Array is mapped as such: arrayIdx = x + y * width + z * width * height.
		 */
		Vector<float> GetDepths() const;

		/**
		 * Constructs a new object with an internal buffer capable of holding "extents" volume of pixels, where each pixel
		 * is of the specified pixel format. Extent offsets are also stored, but are not used internally.
		 */
		static SPtr<PixelData> Create(const PixelVolume& extents, PixelFormat pixelFormat);

		/**
		 * Constructs a new object with an internal buffer capable of holding volume of pixels described by provided width,
		 * height and depth, where each pixel is of the specified pixel format.
		 */
		static SPtr<PixelData> Create(UINT32 width, UINT32 height, UINT32 depth, PixelFormat pixelFormat);

	private:
		/**
		 * Initializes the internal buffer with the provided set of colors. The array should be of width * height * depth
		 * size and mapped as such: arrayIdx = x + y * width + z * width * height.
		 *
		 * @note	A generic method that is reused in other more specific setColors() calls.
		 */
		template<class T>
		void SetColorsInternal(const T& colors, UINT32 numElements);

		/**	Returns the needed size of the internal buffer, in bytes. */
		UINT32 GetInternalBufferSize() const override;

	private:
		PixelVolume _extents = PixelVolume(0, 0, 0, 0);
		PixelFormat _format = PF_UNKNOWN;
		UINT32 _rowPitch = 0;
		UINT32 _slicePitch = 0;
	};
}
