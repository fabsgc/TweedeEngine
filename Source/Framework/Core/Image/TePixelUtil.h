#pragma once

#include "TeCorePrerequisites.h"
#include "Image/TePixelData.h"
#include "Image/TeTexture.h"
#include "Math/TeVector2I.h"

namespace te
{

#undef None

    /** Types of texture compression quality. */
    enum class CompressionQuality
    {
        Fastest,
        Normal,
        Production,
        Highest
    };

    /** Mode of the alpha channel in a texture. */
    enum class AlphaMode
    {
        None, /*< Texture has no alpha values. */
        Transparency, /*< Alpha is in the separate transparency channel. */
        Premultiplied /*< Alpha values have been pre-multiplied with the color values. */
    };

    /** Wrap mode to use when generating mip maps. */
    enum class MipMapWrapMode
    {
        Mirror,
        Repeat,
        Clamp
    };

    /** Filter to use when generating mip maps. */
    enum class MipMapFilter
    {
        Box,
        Triangle,
        Kaiser
    };

    /** Rounding mode to use when generating mip maps */
    enum MipMapRoundMode
    {
        RoundNone,
        ToPreviousPowerOfTwo,
        ToNextPowerOfTwo,
        ToNearestPowerOfTwo
    };

    /** Determines on which axes to mirror an image. */
    enum MirrorMode
    {
        None = 0x00,
        X = 0x01,
        Y = 0x02,
        Z = 0x03
    };

    /** Options used to control texture compression. */
    struct CompressionOptions
    {
        PixelFormat Format = PF_BC1; /*< Format to compress to. Must be a format containing compressed data. */
        AlphaMode Alpha = AlphaMode::None; /*< Controls how to (and if) to compress the alpha channel. */
        bool IsNormalMap = false; /*< Determines does the input data represent a normal map. */
        bool IsSRGB = false; /*< Determines has the input data been gamma corrected. */
        CompressionQuality Quality = CompressionQuality::Normal; /*< Compressed image quality. Better compression might take longer to execute but will generate better results. */
    };

    /** Options used to control texture mip map generation. */
    struct MipMapGenOptions
    {
        AlphaMode Alpha = AlphaMode::None; /*< Controls how to (and if) to compress the alpha channel. */
        MipMapFilter Filter = MipMapFilter::Box; /*< Filter to use when downsamping input data. */
        MipMapWrapMode WrapMode = MipMapWrapMode::Mirror; /*< Determines how to downsample pixels on borders. */
        MipMapRoundMode RoundMode = MipMapRoundMode::RoundNone; /* < Determines how texture must be rounded if dimensions are not a power of */
        CompressionQuality Quality = CompressionQuality::Normal; /*< Compressed image quality. Better compression might take longer to execute but will generate */
        bool IsNormalMap = false; /*< Determines does the input data represent a normal map. */
        bool NormalizeMipmaps = false; /*< Should the downsampled values be re-normalized. Only relevant for mip-maps representing normal maps. */
        bool IsSRGB = false; /*< Determines has the input data been gamma corrected. */
    };

    /** Utility methods for converting and managing pixel data and formats. */
    class TE_CORE_EXPORT PixelUtil
    {
    public:
        /** Filtering types to use when scaling images. */
        enum Filter
        {
            FILTER_NEAREST, /*< No filtering is performed and nearest existing value is used. */
            FILTER_LINEAR /*< Box filter is applied, averaging nearby pixels. */
        };

        /** Returns the size of a single pixel of the provided pixel format, in bytes. */
        static UINT32 GetNumElemBytes(PixelFormat format);

        /**
         * Returns the size of a single compressed block, in bytes. Returns pixel size if the format is not block
         * compressed.
         */
        static UINT32 GetBlockSize(PixelFormat format);

        /**
         * Returns the dimensions of a single compressed block, in number of pixels. Returns 1x1 for non-block-compressed
         * formats.
         */
        static Vector2I GetBlockDimensions(PixelFormat format);

        /** Returns the size of a single pixel of the provided pixel format, in bits. */
        static UINT32 GetNumElemBits(PixelFormat format);

        /** Returns the size of the memory region required to hold pixels of the provided size ana format. */
        static UINT32 GetMemorySize(UINT32 width, UINT32 height, UINT32 depth, PixelFormat format);
        
        /** Calculates the size of a mip level of a texture with the provided size. */
        static void GetSizeForMipLevel(UINT32 width, UINT32 height, UINT32 depth, UINT32 mipLevel,
            UINT32& mipWidth, UINT32& mipHeight, UINT32& mipDepth);

        /**
         * Calculates row and depth pitch for a texture surface of the specified size and format. For most formats row
         * pitch will equal the number of bytes required for storing "width" pixels, and slice pitch will equal the
         * number of bytes required for storing "width*height" pixels. But some texture formats (especially compressed
         * ones) might require extra padding. Input width/height/depth values are in pixels, while output pitch values
         * are in bytes.
         */
        static void GetPitch(UINT32 width, UINT32 height, UINT32 depth, PixelFormat format,
            UINT32& rowPitch, UINT32& depthPitch);

        /**
         * Returns property flags for this pixel format.
         *
         * @see PixelFormatFlags
         */
        static UINT32 GetFlags(PixelFormat format);

        /** Checks if the provided pixel format has an alpha channel. */
        static bool HasAlpha(PixelFormat format);

        /** Checks is the provided pixel format a floating point format. */
        static bool IsFloatingPoint(PixelFormat format);

        /** Checks is the provided pixel format compressed. */
        static bool IsCompressed(PixelFormat format);

        /** Checks is the provided pixel format a depth/stencil buffer format. */
        static bool IsDepth(PixelFormat format);

        /** Checks does the provided format store data in normalized range. */
        static bool IsNormalized(PixelFormat format);

        /**
         * Checks is the provided format valid for the texture type and usage.
         *
         * @param[in, out]	format	Format to check. If format is not valid the method will update this with the closest
         *							relevant format.
         * @param[in]		texType	Type of the texture the format will be used for.
         * @param[in]		usage	A set of TextureUsage flags that define how will a texture be used.
         * @return					True if the format is valid, false if not.
         *
         * @note	This method checks only for obvious format mismatches:
         *			- Using depth format for anything but a depth-stencil buffer
         *			- Using anything but a depth format for a depth-stencil-buffer
         *			- Using compressed format for anything but normal textures
         *			- Using compressed format for 1D textures
         *			
         *			Caller should still check for platform-specific unsupported formats.
         */
        static bool CheckFormat(PixelFormat& format, TextureType texType, int usage);

        /**
         * Checks are the provided dimensions valid for the specified pixel format. Some formats (like BC) require
         * width/height to be multiples of four and some formats dont allow depth larger than 1.
         */
        static bool IsValidExtent(UINT32 width, UINT32 height, UINT32 depth, PixelFormat format);

        /**
         * Returns the number of bits per each element in the provided pixel format. This will return all zero for
         * compressed and depth/stencil formats.
         */
        static void GetBitDepths(PixelFormat format, int(&rgba)[4]);

        /**
         * Returns bit masks that determine in what bit range is each channel stored.
         *
         * @note
         * For example if your color is stored in an UINT32 and you want to extract the red channel you should AND the color
         * UINT32 with the bit-mask for the red channel and then right shift it by the red channel bit shift amount.
         */
        static void GetBitMasks(PixelFormat format, UINT32(&rgba)[4]);

        /**
         * Returns number of bits you need to shift a pixel element in order to move it to the start of the data type.
         *
         * @note
         * For example if your color is stored in an UINT32 and you want to extract the red channel you should AND the color
         * UINT32 with the bit-mask for the red channel and then right shift it by the red channel bit shift amount.
         */
        static void GetBitShifts(PixelFormat format, UINT8 (&rgba)[4]);

        /** Returns the name of the pixel format. */
        static String GetFormatName(PixelFormat srcformat);

        /**
         * Returns true if the pixel data in the format can be directly accessed and read. This is generally not true
         * for compressed formats.
         */
        static bool IsAccessible(PixelFormat srcformat);

        /** Returns the type of an individual pixel element in the provided format. */
        static PixelComponentType GetElementType(PixelFormat format);

        /** Returns the number of pixel elements in the provided format. */
        static UINT32 GetNumElements(PixelFormat format);

        /**
         * Returns the maximum number of mip maps that can be generated until we reach the minimum size possible. This
         * does not count the base level.
         */
        static UINT32 GetMaxMipmaps(UINT32 width, UINT32 height, UINT32 depth);

        /** Writes the color to the provided memory location. */
        static void PackColor(const Color& color, PixelFormat format, void* dest);

        /**
         * Writes the color to the provided memory location. If the destination	format is floating point, the byte values
         * will be converted into [0.0, 1.0] range.
         */
        static void PackColor(UINT8 r, UINT8 g, UINT8 b, UINT8 a, PixelFormat format, void* dest);

        /**
         * Writes the color to the provided memory location. If the destination format in non-floating point, the float
         * values will be assumed to be in [0.0, 1.0] which	will be converted to integer range. ([0, 255] in the case of bytes)
         */
        static void PackColor(float r, float g, float b, float a, const PixelFormat format, void* dest);

        /** Reads the color from the provided memory location and stores it into the provided color object. */
        static void UnpackColor(Color* color, PixelFormat format, const void* src);

        /**
         * Reads the color from the provided memory location and stores it into the provided color elements, as bytes
         * clamped to [0, 255] range.
         */
        static void UnpackColor(UINT8* r, UINT8* g, UINT8* b, UINT8* a, PixelFormat format, const void* src);

        /**
         * Reads the color from the provided memory location and stores it into the provided color elements. If the format
         * is not natively floating point a conversion is done in such a way that returned values range [0.0, 1.0].
         */
        static void UnpackColor(float* r, float* g, float* b, float* a, PixelFormat format, const void* src);

        /** Writes a depth value to the provided memory location. Depth should be in range [0, 1]. */
        static void PackDepth(float depth, const PixelFormat format, void* dest);

        /** Reads the depth from the provided memory location. Value ranges in [0, 1]. */
        static float UnpackDepth(PixelFormat format, void* src);

        /**
         * Converts pixels from one format to another. Provided pixel data objects must have previously allocated buffers
         * of adequate size and their sizes must match.
         */
        static void BulkPixelConversion(const PixelData& src, PixelData& dst);

        /** Flips the order of components in each individual pixel. For example RGBA -> ABGR. */
        static void FlipComponentOrder(PixelData& data);

        /** Compresses the provided data using the specified compression options.  */
        static void Compress(const PixelData& src, PixelData& dst, const CompressionOptions& options);

        /**
         * Generates mip-maps from the provided source data using the specified compression options. Returned list includes
         * the base level.
         *
         * @return	A list of calculated mip-map data. First entry is the largest mip and other follow in order from
         *			largest to smallest.
         */
        static Vector<SPtr<PixelData>> GenMipmaps(const PixelData& src, const MipMapGenOptions& options, UINT32 maxMip = 0);

        /**
         * Generates mip-maps from the provided source data using the specified compression options. Returned list includes
         * the base level.
         *
         * @return	The final texture with all mipmap
         */
        static SPtr<Texture> GenMipmaps(const TEXTURE_DESC& desc, const PixelData& src, const MipMapGenOptions& options, UINT32 maxMip = 0);

        /**
         * Scales pixel data in the source buffer and stores the scaled data in the destination buffer. Provided pixel data
         * objects must have previously allocated buffers of adequate size. You may also provided a filtering method to use
         * when scaling.
         */
        static void Scale(const PixelData& src, PixelData& dst, Filter filter = FILTER_LINEAR);

        /**
         * Mirrors the contents of the provided object along the X, Y and/or Z axes. */
        static void Mirror(PixelData& pixelData, INT32 mode);

        /**
         * Copies the contents of the @p src buffer into the @p dst buffer. The size of the copied contents is determined
         * by the size of the @p dst buffer. First pixel copied from @p src is determined by offset provided in
         * @p offsetX, @p offsetY and @p offsetZ parameters.
         */
        static void Copy(const PixelData& src, PixelData& dst, UINT32 offsetX = 0, UINT32 offsetY = 0, UINT32 offsetZ = 0);
    };
}
