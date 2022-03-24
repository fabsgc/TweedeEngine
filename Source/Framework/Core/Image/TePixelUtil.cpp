#include "Image/TePixelUtil.h"
#include "Image/TeColor.h"
#include "Math/TeMath.h"
#include "Image/TeTexture.h"
#include "Utility/TeBitwise.h"
#include "Renderer/TeTextureDownsampleMat.h"
#include "Renderer/TeTextureCubeDownsampleMat.h"
#include "Renderer/TeRendererUtility.h"
#include "Renderer/TeRendererMaterialManager.h"
#include <nvtt.h>
#include <filesystem>

namespace te
{
    RecursiveMutex PixelUtil::_mutex;

    /**
     * Performs pixel data resampling using the point filter (nearest neighbor). Does not perform format conversions.
     *
     * @tparam elementSize	Size of a single pixel in bytes.
     */
    template<UINT32 elementSize> struct NearestResampler
    {
        static void Scale(const PixelData& source, const PixelData& dest)
        {
            UINT8* sourceData = source.GetData();
            UINT8* destPtr = dest.GetData();

            // Get steps for traversing source data in 16/48 fixed point format
            UINT64 stepX = ((UINT64)source.GetWidth() << 48) / dest.GetWidth();
            UINT64 stepY = ((UINT64)source.GetHeight() << 48) / dest.GetHeight();
            UINT64 stepZ = ((UINT64)source.GetDepth() << 48) / dest.GetDepth();

            UINT64 curZ = (stepZ >> 1) - 1; // Offset half a pixel to start at pixel center
            for (UINT32 z = dest.GetFront(); z < dest.GetBack(); z++, curZ += stepZ)
            {
                UINT32 offsetZ = (UINT32)(curZ >> 48) * source.GetSlicePitch();

                UINT64 curY = (stepY >> 1) - 1; // Offset half a pixel to start at pixel center
                for (UINT32 y = dest.GetTop(); y < dest.GetBottom(); y++, curY += stepY)
                {
                    UINT32 offsetY = (UINT32)(curY >> 48) * source.GetRowPitch();

                    UINT64 curX = (stepX >> 1) - 1; // Offset half a pixel to start at pixel center
                    for (UINT32 x = dest.GetLeft(); x < dest.GetRight(); x++, curX += stepX)
                    {
                        UINT32 offsetX = (UINT32)(curX >> 48);
                        UINT32 offsetBytes = elementSize*offsetX + offsetY + offsetZ;

                        UINT8* curSourcePtr = sourceData + offsetBytes;

                        memcpy(destPtr, curSourcePtr, elementSize);
                        destPtr += elementSize;
                    }

                    destPtr += dest.GetRowSkip();
                }

                destPtr += dest.GetSliceSkip();
            }
        }
    };

    /** Performs pixel data resampling using the box filter (linear). Performs format conversions. */
    struct LinearResampler
    {
        static void Scale(const PixelData& source, const PixelData& dest)
        {
            UINT32 sourceElemSize = PixelUtil::GetNumElemBytes(source.GetFormat());
            UINT32 destElemSize = PixelUtil::GetNumElemBytes(dest.GetFormat());

            UINT8* sourceData = source.GetData();
            UINT8* destPtr = dest.GetData();

            // Get steps for traversing source data in 16/48 fixed point precision format
            UINT64 stepX = ((UINT64)source.GetWidth() << 48) / dest.GetWidth();
            UINT64 stepY = ((UINT64)source.GetHeight() << 48) / dest.GetHeight();
            UINT64 stepZ = ((UINT64)source.GetDepth() << 48) / dest.GetDepth();

            // Contains 16/16 fixed point precision format. Most significant
            // 16 bits will contain the coordinate in the source image, and the
            // least significant 16 bits will contain the fractional part of the coordinate
            // that will be used for determining the blend amount.
            UINT32 temp = 0;

            UINT64 curZ = (stepZ >> 1) - 1; // Offset half a pixel to start at pixel center
            for (UINT32 z = dest.GetFront(); z < dest.GetBack(); z++, curZ += stepZ)
            {
                temp = UINT32(curZ >> 32);
                temp = (temp > 0x8000)? temp - 0x8000 : 0;
                UINT32 sampleCoordZ1 = temp >> 16;
                UINT32 sampleCoordZ2 = std::min(sampleCoordZ1 + 1, (UINT32)source.GetDepth() - 1);
                float sampleWeightZ = (temp & 0xFFFF) / 65536.0f;

                UINT64 curY = (stepY >> 1) - 1; // Offset half a pixel to start at pixel center
                for (UINT32 y = dest.GetTop(); y < dest.GetBottom(); y++, curY += stepY)
                {
                    temp = (UINT32)(curY >> 32);
                    temp = (temp > 0x8000)? temp - 0x8000 : 0;
                    UINT32 sampleCoordY1 = temp >> 16;
                    UINT32 sampleCoordY2 = std::min(sampleCoordY1 + 1, (UINT32)source.GetHeight() - 1);
                    float sampleWeightY = (temp & 0xFFFF) / 65536.0f;

                    UINT64 curX = (stepX >> 1) - 1; // Offset half a pixel to start at pixel center
                    for (UINT32 x = dest.GetLeft(); x < dest.GetRight(); x++, curX += stepX)
                    {
                        temp = (UINT32)(curX >> 32);
                        temp = (temp > 0x8000)? temp - 0x8000 : 0;
                        UINT32 sampleCoordX1 = temp >> 16;
                        UINT32 sampleCoordX2 = std::min(sampleCoordX1 + 1, (UINT32)source.GetWidth() - 1);
                        float sampleWeightX = (temp & 0xFFFF) / 65536.0f;

                        Color x1y1z1, x2y1z1, x1y2z1, x2y2z1;
                        Color x1y1z2, x2y1z2, x1y2z2, x2y2z2;

#define GETSOURCEDATA(x, y, z) sourceData + sourceElemSize*(x)+(y)*source.GetRowPitch() + (z)*source.GetSlicePitch()

                        PixelUtil::UnpackColor(&x1y1z1, source.GetFormat(), GETSOURCEDATA(sampleCoordX1, sampleCoordY1, sampleCoordZ1));
                        PixelUtil::UnpackColor(&x2y1z1, source.GetFormat(), GETSOURCEDATA(sampleCoordX2, sampleCoordY1, sampleCoordZ1));
                        PixelUtil::UnpackColor(&x1y2z1, source.GetFormat(), GETSOURCEDATA(sampleCoordX1, sampleCoordY2, sampleCoordZ1));
                        PixelUtil::UnpackColor(&x2y2z1, source.GetFormat(), GETSOURCEDATA(sampleCoordX2, sampleCoordY2, sampleCoordZ1));
                        PixelUtil::UnpackColor(&x1y1z2, source.GetFormat(), GETSOURCEDATA(sampleCoordX1, sampleCoordY1, sampleCoordZ2));
                        PixelUtil::UnpackColor(&x2y1z2, source.GetFormat(), GETSOURCEDATA(sampleCoordX2, sampleCoordY1, sampleCoordZ2));
                        PixelUtil::UnpackColor(&x1y2z2, source.GetFormat(), GETSOURCEDATA(sampleCoordX1, sampleCoordY2, sampleCoordZ2));
                        PixelUtil::UnpackColor(&x2y2z2, source.GetFormat(), GETSOURCEDATA(sampleCoordX2, sampleCoordY2, sampleCoordZ2));
#undef GETSOURCEDATA

                        Color accum =
                            x1y1z1 * ((1.0f - sampleWeightX)*(1.0f - sampleWeightY)*(1.0f - sampleWeightZ)) +
                            x2y1z1 * (        sampleWeightX *(1.0f - sampleWeightY)*(1.0f - sampleWeightZ)) +
                            x1y2z1 * ((1.0f - sampleWeightX)*        sampleWeightY *(1.0f - sampleWeightZ)) +
                            x2y2z1 * (        sampleWeightX *        sampleWeightY *(1.0f - sampleWeightZ)) +
                            x1y1z2 * ((1.0f - sampleWeightX)*(1.0f - sampleWeightY)*        sampleWeightZ ) +
                            x2y1z2 * (        sampleWeightX *(1.0f - sampleWeightY)*        sampleWeightZ ) +
                            x1y2z2 * ((1.0f - sampleWeightX)*        sampleWeightY *        sampleWeightZ ) +
                            x2y2z2 * (        sampleWeightX *        sampleWeightY *        sampleWeightZ );

                        PixelUtil::PackColor(accum, dest.GetFormat(), destPtr);

                        destPtr += destElemSize;
                    }

                    destPtr += dest.GetRowSkip();
                }

                destPtr += dest.GetSliceSkip();
            }
        }
    };

    /**
     * Performs pixel data resampling using the box filter (linear). Only handles float RGB or RGBA pixel data (32 bits per
     * channel).
     */
    struct LinearResampler_Float32
    {
        static void Scale(const PixelData& source, const PixelData& dest)
        {
            UINT32 sourcePixelSize = PixelUtil::GetNumElemBytes(source.GetFormat());
            UINT32 destPixelSize = PixelUtil::GetNumElemBytes(dest.GetFormat());

            UINT32 numSourceChannels = sourcePixelSize / sizeof(float);
            UINT32 numDestChannels = destPixelSize / sizeof(float);

            float* sourceData = (float*)source.GetData();
            float* destPtr = (float*)dest.GetData();

            // Get steps for traversing source data in 16/48 fixed point precision format
            UINT64 stepX = ((UINT64)source.GetWidth() << 48) / dest.GetWidth();
            UINT64 stepY = ((UINT64)source.GetHeight() << 48) / dest.GetHeight();
            UINT64 stepZ = ((UINT64)source.GetDepth() << 48) / dest.GetDepth();

            UINT32 sourceRowPitch = source.GetRowPitch() / sourcePixelSize;
            UINT32 sourceSlicePitch = source.GetSlicePitch() / sourcePixelSize;

            // Contains 16/16 fixed point precision format. Most significant
            // 16 bits will contain the coordinate in the source image, and the
            // least significant 16 bits will contain the fractional part of the coordinate
            // that will be used for determining the blend amount.
            UINT32 temp = 0;

            UINT64 curZ = (stepZ >> 1) - 1; // Offset half a pixel to start at pixel center
            for (UINT32 z = dest.GetFront(); z < dest.GetBack(); z++, curZ += stepZ)
            {
                temp = (UINT32)(curZ >> 32);
                temp = (temp > 0x8000)? temp - 0x8000 : 0;
                UINT32 sampleCoordZ1 = temp >> 16;
                UINT32 sampleCoordZ2 = std::min(sampleCoordZ1 + 1, (UINT32)source.GetDepth() - 1);
                float sampleWeightZ = (temp & 0xFFFF) / 65536.0f;

                UINT64 curY = (stepY >> 1) - 1; // Offset half a pixel to start at pixel center
                for (UINT32 y = dest.GetTop(); y < dest.GetBottom(); y++, curY += stepY)
                {
                    temp = (UINT32)(curY >> 32);
                    temp = (temp > 0x8000)? temp - 0x8000 : 0;
                    UINT32 sampleCoordY1 = temp >> 16;
                    UINT32 sampleCoordY2 = std::min(sampleCoordY1 + 1, (UINT32)source.GetHeight() - 1);
                    float sampleWeightY = (temp & 0xFFFF) / 65536.0f;

                    UINT64 curX = (stepX >> 1) - 1; // Offset half a pixel to start at pixel center
                    for (UINT32 x = dest.GetLeft(); x < dest.GetRight(); x++, curX += stepX)
                    {
                        temp = (UINT32)(curX >> 32);
                        temp = (temp > 0x8000)? temp - 0x8000 : 0;
                        UINT32 sampleCoordX1 = temp >> 16;
                        UINT32 sampleCoordX2 = std::min(sampleCoordX1 + 1, (UINT32)source.GetWidth() - 1);
                        float sampleWeightX = (temp & 0xFFFF) / 65536.0f;

                        // process R,G,B,A simultaneously for cache coherence?
                        float accum[4] = { 0.0f, 0.0f, 0.0f, 0.0f };


#define ACCUM3(x,y,z,factor) \
                        { float f = factor; \
                        UINT32 offset = (x + y*sourceRowPitch + z*sourceSlicePitch)*numSourceChannels; \
                        accum[0] += sourceData[offset + 0] * f; accum[1] += sourceData[offset + 1] * f; \
                        accum[2] += sourceData[offset + 2] * f; }

#define ACCUM4(x,y,z,factor) \
                        { float f = factor; \
                        UINT32 offset = (x + y*sourceRowPitch + z*sourceSlicePitch)*numSourceChannels; \
                        accum[0] += sourceData[offset + 0] * f; accum[1] += sourceData[offset + 1] * f; \
                        accum[2] += sourceData[offset + 2] * f; accum[3] += sourceData[offset + 3] * f; }

                        if (numSourceChannels == 3 || numDestChannels == 3)
                        {
                            // RGB
                            ACCUM3(sampleCoordX1, sampleCoordY1, sampleCoordZ1, (1.0f - sampleWeightX) * (1.0f - sampleWeightY) * (1.0f - sampleWeightZ));
                            ACCUM3(sampleCoordX2, sampleCoordY1, sampleCoordZ1, sampleWeightX		   * (1.0f - sampleWeightY) * (1.0f - sampleWeightZ));
                            ACCUM3(sampleCoordX1, sampleCoordY2, sampleCoordZ1, (1.0f - sampleWeightX) * sampleWeightY			* (1.0f - sampleWeightZ));
                            ACCUM3(sampleCoordX2, sampleCoordY2, sampleCoordZ1, sampleWeightX		   * sampleWeightY		    * (1.0f - sampleWeightZ));
                            ACCUM3(sampleCoordX1, sampleCoordY1, sampleCoordZ2, (1.0f - sampleWeightX) * (1.0f - sampleWeightY) * sampleWeightZ);
                            ACCUM3(sampleCoordX2, sampleCoordY1, sampleCoordZ2, sampleWeightX		   * (1.0f - sampleWeightY) * sampleWeightZ);
                            ACCUM3(sampleCoordX1, sampleCoordY2, sampleCoordZ2, (1.0f - sampleWeightX) * sampleWeightY			* sampleWeightZ);
                            ACCUM3(sampleCoordX2, sampleCoordY2, sampleCoordZ2, sampleWeightX		   * sampleWeightY			* sampleWeightZ);
                            accum[3] = 1.0f;
                        }
                        else
                        {
                            // RGBA
                            ACCUM4(sampleCoordX1, sampleCoordY1, sampleCoordZ1, (1.0f - sampleWeightX) * (1.0f - sampleWeightY) * (1.0f - sampleWeightZ));
                            ACCUM4(sampleCoordX2, sampleCoordY1, sampleCoordZ1, sampleWeightX		   * (1.0f - sampleWeightY) * (1.0f - sampleWeightZ));
                            ACCUM4(sampleCoordX1, sampleCoordY2, sampleCoordZ1, (1.0f - sampleWeightX) * sampleWeightY			* (1.0f - sampleWeightZ));
                            ACCUM4(sampleCoordX2, sampleCoordY2, sampleCoordZ1, sampleWeightX		   * sampleWeightY			* (1.0f - sampleWeightZ));
                            ACCUM4(sampleCoordX1, sampleCoordY1, sampleCoordZ2, (1.0f - sampleWeightX) * (1.0f - sampleWeightY) * sampleWeightZ);
                            ACCUM4(sampleCoordX2, sampleCoordY1, sampleCoordZ2, sampleWeightX		   * (1.0f - sampleWeightY) * sampleWeightZ);
                            ACCUM4(sampleCoordX1, sampleCoordY2, sampleCoordZ2, (1.0f - sampleWeightX) * sampleWeightY			* sampleWeightZ);
                            ACCUM4(sampleCoordX2, sampleCoordY2, sampleCoordZ2, sampleWeightX		   * sampleWeightY			* sampleWeightZ);
                        }

                        memcpy(destPtr, accum, sizeof(float)*numDestChannels);

#undef ACCUM3
#undef ACCUM4

                        destPtr += numDestChannels;
                    }

                    destPtr += dest.GetRowSkip() / sizeof(float);
                }

                destPtr += dest.GetSliceSkip() / sizeof(float);
            }
        }
    };

    // byte linear resampler, does not do any format conversions.
    // only handles pixel formats that use 1 byte per color channel.
    // 2D only; punts 3D pixelboxes to default LinearResampler (slow).
    // templated on bytes-per-pixel to allow compiler optimizations, such
    // as unrolling loops and replacing multiplies with bitshifts

    /**
     * Performs pixel data resampling using the box filter (linear). Only handles pixel formats with one byte per channel.
     * Does not perform format conversion.
     *
     * @tparam	channels	Number of channels in the pixel format.
     */
    template<UINT32 channels> struct LinearResampler_Byte
    {
        static void Scale(const PixelData& source, const PixelData& dest)
        {
            // Only optimized for 2D
            if (source.GetDepth() > 1 || dest.GetDepth() > 1)
            {
                LinearResampler::Scale(source, dest);
                return;
            }

            UINT8* sourceData = (UINT8*)source.GetData();
            UINT8* destPtr = (UINT8*)dest.GetData();

            // Get steps for traversing source data in 16/48 fixed point precision format
            UINT64 stepX = ((UINT64)source.GetWidth() << 48) / dest.GetWidth();
            UINT64 stepY = ((UINT64)source.GetHeight() << 48) / dest.GetHeight();

            // Contains 16/16 fixed point precision format. Most significant
            // 16 bits will contain the coordinate in the source image, and the
            // least significant 16 bits will contain the fractional part of the coordinate
            // that will be used for determining the blend amount.
            UINT32 temp;

            UINT64 curY = (stepY >> 1) - 1; // Offset half a pixel to start at pixel center
            for (UINT32 y = dest.GetTop(); y < dest.GetBottom(); y++, curY += stepY)
            {
                temp = (UINT32)(curY >> 36);
                temp = (temp > 0x800)? temp - 0x800: 0;
                UINT32 sampleWeightY = temp & 0xFFF;
                UINT32 sampleCoordY1 = temp >> 12;
                UINT32 sampleCoordY2 = std::min(sampleCoordY1 + 1, (UINT32)source.GetBottom() - source.GetTop() - 1);

                UINT32 sampleY1Offset = sampleCoordY1 * source.GetRowPitch();
                UINT32 sampleY2Offset = sampleCoordY2 * source.GetRowPitch();

                UINT64 curX = (stepX >> 1) - 1; // Offset half a pixel to start at pixel center
                for (UINT32 x = dest.GetLeft(); x < dest.GetRight(); x++, curX += stepX)
                {
                    temp = (UINT32)(curX >> 36);
                    temp = (temp > 0x800)? temp - 0x800 : 0;
                    UINT32 sampleWeightX = temp & 0xFFF;
                    UINT32 sampleCoordX1 = temp >> 12;
                    UINT32 sampleCoordX2 = std::min(sampleCoordX1 + 1, (UINT32)source.GetRight() - source.GetLeft() - 1);

                    UINT32 sxfsyf = sampleWeightX*sampleWeightY;
                    for (UINT32 k = 0; k < channels; k++)
                    {
                        UINT32 accum =
                            sourceData[sampleCoordX1 * channels + sampleY1Offset + k]*(0x1000000-(sampleWeightX<<12)-(sampleWeightY<<12)+sxfsyf) +
                            sourceData[sampleCoordX2 * channels + sampleY1Offset + k]*((sampleWeightX<<12)-sxfsyf) +
                            sourceData[sampleCoordX1 * channels + sampleY2Offset + k]*((sampleWeightY<<12)-sxfsyf) +
                            sourceData[sampleCoordX2 * channels + sampleY2Offset + k]*sxfsyf;

                        // Round up to byte size
                        *destPtr = (UINT8)((accum + 0x800000) >> 24);
                        destPtr++;
                    }
                }
                destPtr += dest.GetRowSkip();
            }
        }
    };

    /**	Data describing a pixel format. */
    struct PixelFormatDescription
    {
        const char* name; /**< Name of the format. */
        UINT8 elemBytes; /**< Number of bytes one element (color value) uses. */
        UINT32 flags; /**< PixelFormatFlags set by the pixel format. */
        PixelComponentType componentType; /**< Data type of a single element of the format. */
        UINT8 componentCount; /**< Number of elements in the format. */

        UINT8 rbits, gbits, bbits, abits; /**< Number of bits per element in the format. */

        UINT32 rmask, gmask, bmask, amask; /**< Masks used by packers/unpackers. */
        UINT8 rshift, gshift, bshift, ashift; /**< Shifts used by packers/unpackers. */
    };

    /**	A list of all available pixel formats. */
    PixelFormatDescription _pixelFormats[PF_COUNT] = {
        {"PF_UNKNOWN",
        /* Bytes per element */
        0,
        /* Flags */
        0,
        /* Component type and count */
        PCT_BYTE, 0,
        /* rbits, gbits, bbits, abits */
        0, 0, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        {"PF_R8",
        /* Bytes per element */
        1,
        /* Flags */
        PFF_INTEGER | PFF_NORMALIZED,
        /* Component type and count */
        PCT_BYTE, 1,
        /* rbits, gbits, bbits, abits */
        8, 0, 0, 0,
        /* Masks and shifts */
        0x000000FF, 0, 0, 0,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        {"PF_RG8",
        /* Bytes per element */
        2,
        /* Flags */
        PFF_INTEGER | PFF_NORMALIZED,
        /* Component type and count */
        PCT_BYTE, 2,
        /* rbits, gbits, bbits, abits */
        8, 8, 0, 0,
        /* Masks and shifts */
        0x000000FF, 0x0000FF00, 0, 0,
        0, 8, 0, 0,
        },
    //-----------------------------------------------------------------------
        {"PF_RGB8",
        /* Bytes per element */
        4,  // 4th byte is unused
        /* Flags */
        PFF_INTEGER | PFF_NORMALIZED,
        /* Component type and count */
        PCT_BYTE, 3,
        /* rbits, gbits, bbits, abits */
        8, 8, 8, 0,
        /* Masks and shifts */
        0x000000FF, 0x0000FF00, 0x00FF0000, 0,
        0, 8, 16, 0,
        },
    //-----------------------------------------------------------------------
        {"PF_BGR8",
        /* Bytes per element */
        4,  // 4th byte is unused
        /* Flags */
        PFF_INTEGER | PFF_NORMALIZED,
        /* Component type and count */
        PCT_BYTE, 3,
        /* rbits, gbits, bbits, abits */
        8, 8, 8, 0,
        /* Masks and shifts */
        0x00FF0000, 0x0000FF00, 0x000000FF, 0,
        16, 8, 0, 0,
        },
    //-----------------------------------------------------------------------
        {}, // Deleted format
    //-----------------------------------------------------------------------
        {}, // Deleted format
    //-----------------------------------------------------------------------
        {"PF_BGRA8",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_HASALPHA | PFF_INTEGER | PFF_NORMALIZED,
        /* Component type and count */
        PCT_BYTE, 4,
        /* rbits, gbits, bbits, abits */
        8, 8, 8, 8,
        /* Masks and shifts */
        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000,
        16, 8, 0, 24,
        },
    //-----------------------------------------------------------------------
        {"PF_RGBA8",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_HASALPHA | PFF_INTEGER | PFF_NORMALIZED,
        /* Component type and count */
        PCT_BYTE, 4,
        /* rbits, gbits, bbits, abits */
        8, 8, 8, 8,
        /* Masks and shifts */
        0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000,
        0, 8, 16, 24,
        },
    //-----------------------------------------------------------------------
        {}, // Deleted format
    //-----------------------------------------------------------------------
        {}, // Deleted format
    //-----------------------------------------------------------------------
        {}, // Deleted format
    //-----------------------------------------------------------------------
        {}, // Deleted format
    //-----------------------------------------------------------------------
        { "PF_BC1",
        /* Bytes per element */
        0,
        /* Flags */
        PFF_COMPRESSED | PFF_HASALPHA,
        /* Component type and count */
        PCT_BYTE, 3, // No alpha
        /* rbits, gbits, bbits, abits */
        0, 0, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_BC1a",
        /* Bytes per element */
        0,
        /* Flags */
        PFF_COMPRESSED,
        /* Component type and count */
        PCT_BYTE, 3,
        /* rbits, gbits, bbits, abits */
        0, 0, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_BC2",
        /* Bytes per element */
        0,
        /* Flags */
        PFF_COMPRESSED | PFF_HASALPHA,
        /* Component type and count */
        PCT_BYTE, 4,
        /* rbits, gbits, bbits, abits */
        0, 0, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_BC3",
        /* Bytes per element */
        0,
        /* Flags */
        PFF_COMPRESSED | PFF_HASALPHA,
        /* Component type and count */
        PCT_BYTE, 4,
        /* rbits, gbits, bbits, abits */
        0, 0, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_BC4",
        /* Bytes per element */
        0,
        /* Flags */
        PFF_COMPRESSED,
        /* Component type and count */
        PCT_BYTE, 1,
        /* rbits, gbits, bbits, abits */
        0, 0, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_BC5",
        /* Bytes per element */
        0,
        /* Flags */
        PFF_COMPRESSED,
        /* Component type and count */
        PCT_BYTE, 2,
        /* rbits, gbits, bbits, abits */
        0, 0, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_BC6H",
        /* Bytes per element */
        0,
        /* Flags */
        PFF_COMPRESSED,
        /* Component type and count */
        PCT_FLOAT16, 3,
        /* rbits, gbits, bbits, abits */
        0, 0, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_BC7",
        /* Bytes per element */
        0,
        /* Flags */
        PFF_COMPRESSED | PFF_HASALPHA,
        /* Component type and count */
        PCT_BYTE, 4,
        /* rbits, gbits, bbits, abits */
        0, 0, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        {"PF_R16F",
        /* Bytes per element */
        2,
        /* Flags */
        PFF_FLOAT,
        /* Component type and count */
        PCT_FLOAT16, 1,
        /* rbits, gbits, bbits, abits */
        16, 0, 0, 0,
        /* Masks and shifts */
        0x0000FFFF, 0, 0, 0,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        {"PF_RG16F",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_FLOAT,
        /* Component type and count */
        PCT_FLOAT16, 2,
        /* rbits, gbits, bbits, abits */
        16, 16, 0, 0,
        /* Masks and shifts */
        0x0000FFFF, 0xFFFF0000, 0, 0,
        0, 16, 0, 0,
        },
    //-----------------------------------------------------------------------
        { }, //  Deleted format
    //-----------------------------------------------------------------------
        { "PF_RGBA16F",
        /* Bytes per element */
        8,
        /* Flags */
        PFF_FLOAT | PFF_HASALPHA,
        /* Component type and count */
        PCT_FLOAT16, 4,
        /* rbits, gbits, bbits, abits */
        16, 16, 16, 16,
        /* Masks and shifts */
        0x0000FFFF, 0xFFFF0000, 0x0000FFFF, 0xFFFF0000,
        0, 16, 0, 16,
        },
    //-----------------------------------------------------------------------
        {"PF_R32F",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_FLOAT,
        /* Component type and count */
        PCT_FLOAT32, 1,
        /* rbits, gbits, bbits, abits */
        32, 0, 0, 0,
        /* Masks and shifts */
        0xFFFFFFFF, 0, 0, 0,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        {"PF_RG32F",
        /* Bytes per element */
        8,
        /* Flags */
        PFF_FLOAT,
        /* Component type and count */
        PCT_FLOAT32, 2,
        /* rbits, gbits, bbits, abits */
        32, 32, 0, 0,
        /* Masks and shifts */
        0xFFFFFFFF, 0xFFFFFFFF, 0, 0,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_RGB32F",
        /* Bytes per element */
        12,
        /* Flags */
        PFF_FLOAT,
        /* Component type and count */
        PCT_FLOAT32, 3,
        /* rbits, gbits, bbits, abits */
        32, 32, 32, 0,
        /* Masks and shifts */
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_RGBA32F",
        /* Bytes per element */
        16,
        /* Flags */
        PFF_FLOAT | PFF_HASALPHA,
        /* Component type and count */
        PCT_FLOAT32, 4,
        /* rbits, gbits, bbits, abits */
        32, 32, 32, 32,
        /* Masks and shifts */
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        {"PF_D32_S8X24",
        /* Bytes per element */
        8,
        /* Flags */
        PFF_DEPTH | PFF_NORMALIZED,
        /* Component type and count */
        PCT_FLOAT32, 2,
        /* rbits, gbits, bbits, abits */
        32, 8, 0, 0,
        /* Masks and shifts */
        0xFFFFFFFF, 0x000000FF, 0x00000000, 0x00000000,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        {"PF_D24_S8",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_DEPTH | PFF_INTEGER | PFF_NORMALIZED,
        /* Component type and count */
        PCT_INT, 2,
        /* rbits, gbits, bbits, abits */
        24, 8, 0, 0,
        /* Masks and shifts */
        0x00FFFFFF, 0x0FF0000, 0x00000000, 0x00000000,
        0, 24, 0, 0,
        },
    //-----------------------------------------------------------------------
        {"PF_D32",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_DEPTH | PFF_FLOAT,
        /* Component type and count */
        PCT_FLOAT32, 1,
        /* rbits, gbits, bbits, abits */
        32, 0, 0, 0,
        /* Masks and shifts */
        0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        {"PF_D16",
        /* Bytes per element */
        2,
        /* Flags */
        PFF_DEPTH | PFF_INTEGER | PFF_NORMALIZED,
        /* Component type and count */
        PCT_SHORT, 1,
        /* rbits, gbits, bbits, abits */
        16, 0, 0, 0,
        /* Masks and shifts */
        0x0000FFFF, 0x00000000, 0x00000000, 0x00000000,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_RG11B10F",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_FLOAT,
        /* Component type and count */
        PCT_PACKED_R11G11B10, 1,
        /* rbits, gbits, bbits, abits */
        11, 11, 10, 0,
        /* Masks and shifts */
        0x000007FF, 0x003FF800, 0xFFC00000, 0,
        0, 11, 22, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_RGB10A2",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_INTEGER | PFF_NORMALIZED | PFF_HASALPHA,
        /* Component type and count */
        PCT_PACKED_R10G10B10A2, 1,
        /* rbits, gbits, bbits, abits */
        10, 10, 10, 2,
        /* Masks and shifts */
        0x000003FF, 0x000FFC00, 0x3FF00000, 0xC0000000,
        0, 10, 20, 30,
        },
    //-----------------------------------------------------------------------
        { "PF_R8I",
        /* Bytes per element */
        1,
        /* Flags */
        PFF_INTEGER | PFF_SIGNED,
        /* Component type and count */
        PCT_BYTE, 1,
        /* rbits, gbits, bbits, abits */
        8, 0, 0, 0,
        /* Masks and shifts */
        0x000000FF, 0, 0, 0,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_RG8I",
        /* Bytes per element */
        2,
        /* Flags */
        PFF_INTEGER | PFF_SIGNED,
        /* Component type and count */
        PCT_BYTE, 2,
        /* rbits, gbits, bbits, abits */
        8, 8, 0, 0,
        /* Masks and shifts */
        0x000000FF, 0x0000FF00, 0, 0,
        0, 8, 0, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_RGBA8I",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_INTEGER | PFF_SIGNED | PFF_HASALPHA,
        /* Component type and count */
        PCT_BYTE, 4,
        /* rbits, gbits, bbits, abits */
        8, 8, 8, 8,
        /* Masks and shifts */
        0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000,
        0, 8, 16, 24,
        },
    //-----------------------------------------------------------------------
        { "PF_R8U",
        /* Bytes per element */
        1,
        /* Flags */
        PFF_INTEGER,
        /* Component type and count */
        PCT_BYTE, 1,
        /* rbits, gbits, bbits, abits */
        8, 0, 0, 0,
        /* Masks and shifts */
        0x000000FF, 0, 0, 0,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_RG8U",
        /* Bytes per element */
        2,
        /* Flags */
        PFF_INTEGER,
        /* Component type and count */
        PCT_BYTE, 2,
        /* rbits, gbits, bbits, abits */
        8, 8, 0, 0,
        /* Masks and shifts */
        0x000000FF, 0x0000FF00, 0, 0,
        0, 8, 0, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_RGBA8U",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_INTEGER | PFF_HASALPHA,
        /* Component type and count */
        PCT_BYTE, 4,
        /* rbits, gbits, bbits, abits */
        8, 8, 8, 8,
        /* Masks and shifts */
        0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000,
        0, 8, 16, 24,
        },
    //-----------------------------------------------------------------------
        { "PF_R8S",
        /* Bytes per element */
        1,
        /* Flags */
        PFF_INTEGER | PFF_NORMALIZED | PFF_SIGNED,
        /* Component type and count */
        PCT_BYTE, 1,
        /* rbits, gbits, bbits, abits */
        8, 0, 0, 0,
        /* Masks and shifts */
        0x000000FF, 0, 0, 0,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_RG8S",
        /* Bytes per element */
        2,
        /* Flags */
        PFF_INTEGER | PFF_NORMALIZED | PFF_SIGNED,
        /* Component type and count */
        PCT_BYTE, 2,
        /* rbits, gbits, bbits, abits */
        8, 8, 0, 0,
        /* Masks and shifts */
        0x000000FF, 0x0000FF00, 0, 0,
        0, 8, 0, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_RGBA8S",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_INTEGER | PFF_NORMALIZED | PFF_SIGNED | PFF_HASALPHA,
        /* Component type and count */
        PCT_BYTE, 4,
        /* rbits, gbits, bbits, abits */
        8, 8, 8, 8,
        /* Masks and shifts */
        0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000,
        0, 8, 16, 24,
        },
    //-----------------------------------------------------------------------
        { "PF_R16I",
        /* Bytes per element */
        2,
        /* Flags */
        PFF_INTEGER | PFF_SIGNED,
        /* Component type and count */
        PCT_SHORT, 1,
        /* rbits, gbits, bbits, abits */
        16, 0, 0, 0,
        /* Masks and shifts */
        0x0000FFFF, 0, 0, 0,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_RG16I",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_INTEGER | PFF_SIGNED,
        /* Component type and count */
        PCT_SHORT, 2,
        /* rbits, gbits, bbits, abits */
        16, 16, 0, 0,
        /* Masks and shifts */
        0x0000FFFF, 0xFFFF0000, 0, 0,
        0, 16, 0, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_RGBA16I",
        /* Bytes per element */
        8,
        /* Flags */
        PFF_INTEGER | PFF_SIGNED | PFF_HASALPHA,
        /* Component type and count */
        PCT_SHORT, 4,
        /* rbits, gbits, bbits, abits */
        16, 16, 16, 16,
        /* Masks and shifts */
        0x0000FFFF, 0xFFFF0000, 0x0000FFFF, 0xFFFF0000,
        0, 16, 0, 16,
        },
    //-----------------------------------------------------------------------
        { "PF_R16U",
        /* Bytes per element */
        2,
        /* Flags */
        PFF_INTEGER,
        /* Component type and count */
        PCT_SHORT, 1,
        /* rbits, gbits, bbits, abits */
        16, 0, 0, 0,
        /* Masks and shifts */
        0x0000FFFF, 0, 0, 0,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_RG16U",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_INTEGER,
        /* Component type and count */
        PCT_SHORT, 2,
        /* rbits, gbits, bbits, abits */
        16, 16, 0, 0,
        /* Masks and shifts */
        0x0000FFFF, 0xFFFF0000, 0, 0,
        0, 16, 0, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_RGBA16U",
        /* Bytes per element */
        8,
        /* Flags */
        PFF_INTEGER | PFF_HASALPHA,
        /* Component type and count */
        PCT_SHORT, 4,
        /* rbits, gbits, bbits, abits */
        16, 16, 16, 16,
        /* Masks and shifts */
        0x0000FFFF, 0xFFFF0000, 0x0000FFFF, 0xFFFF0000,
        0, 16, 0, 16,
        },
    //-----------------------------------------------------------------------
        { "PF_R32I",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_INTEGER,
        /* Component type and count */
        PCT_INT, 1,
        /* rbits, gbits, bbits, abits */
        32, 0, 0, 0,
        /* Masks and shifts */
        0xFFFFFFFF, 0, 0, 0,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_RG32I",
        /* Bytes per element */
        8,
        /* Flags */
        PFF_INTEGER | PFF_SIGNED,
        /* Component type and count */
        PCT_INT, 2,
        /* rbits, gbits, bbits, abits */
        32, 32, 0, 0,
        /* Masks and shifts */
        0xFFFFFFFF, 0xFFFFFFFF, 0, 0,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_RGB32I",
        /* Bytes per element */
        12,
        /* Flags */
        PFF_INTEGER | PFF_SIGNED,
        /* Component type and count */
        PCT_INT, 3,
        /* rbits, gbits, bbits, abits */
        32, 32, 32, 0,
        /* Masks and shifts */
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_RGBA32I",
        /* Bytes per element */
        16,
        /* Flags */
        PFF_INTEGER | PFF_SIGNED | PFF_HASALPHA,
        /* Component type and count */
        PCT_INT, 4,
        /* rbits, gbits, bbits, abits */
        32, 32, 32, 32,
        /* Masks and shifts */
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
        0, 0, 0, 0
        },
    //-----------------------------------------------------------------------
        { "PF_R32U",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_INTEGER,
        /* Component type and count */
        PCT_INT, 1,
        /* rbits, gbits, bbits, abits */
        32, 0, 0, 0,
        /* Masks and shifts */
        0xFFFFFFFF, 0, 0, 0,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_RG32U",
        /* Bytes per element */
        8,
        /* Flags */
        PFF_INTEGER,
        /* Component type and count */
        PCT_INT, 2,
        /* rbits, gbits, bbits, abits */
        32, 32, 0, 0,
        /* Masks and shifts */
        0xFFFFFFFF, 0xFFFFFFFF, 0, 0,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_RGB32U",
        /* Bytes per element */
        12,
        /* Flags */
        PFF_INTEGER,
        /* Component type and count */
        PCT_INT, 3,
        /* rbits, gbits, bbits, abits */
        32, 32, 32, 0,
        /* Masks and shifts */
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_RGBA32U",
        /* Bytes per element */
        16,
        /* Flags */
        PFF_INTEGER | PFF_HASALPHA,
        /* Component type and count */
        PCT_INT, 4,
        /* rbits, gbits, bbits, abits */
        32, 32, 32, 32,
        /* Masks and shifts */
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
        0, 0, 0, 0
        },
    //-----------------------------------------------------------------------
        { "PF_R16S",
        /* Bytes per element */
        2,
        /* Flags */
        PFF_INTEGER | PFF_NORMALIZED | PFF_SIGNED,
        /* Component type and count */
        PCT_SHORT, 1,
        /* rbits, gbits, bbits, abits */
        16, 0, 0, 0,
        /* Masks and shifts */
        0x0000FFFF, 0, 0, 0,
        0, 0, 0, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_RG16S",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_INTEGER | PFF_NORMALIZED | PFF_SIGNED,
        /* Component type and count */
        PCT_SHORT, 2,
        /* rbits, gbits, bbits, abits */
        16, 16, 0, 0,
        /* Masks and shifts */
        0x0000FFFF, 0xFFFF0000, 0, 0,
        0, 16, 0, 0,
        },
    //-----------------------------------------------------------------------
        { "PF_RGBA16S",
        /* Bytes per element */
        8,
        /* Flags */
        PFF_INTEGER | PFF_NORMALIZED | PFF_SIGNED | PFF_HASALPHA,
        /* Component type and count */
        PCT_SHORT, 4,
        /* rbits, gbits, bbits, abits */
        16, 16, 16, 16,
        /* Masks and shifts */
        0x0000FFFF, 0xFFFF0000, 0x0000FFFF, 0xFFFF0000,
        0, 16, 0, 16,
        },
    //-----------------------------------------------------------------------
        { "PF_R16",
        /* Bytes per element */
        2,
        /* Flags */
        PFF_INTEGER | PFF_NORMALIZED,
        /* Component type and count */
        PCT_SHORT, 1,
        /* rbits, gbits, bbits, abits */
        16, 0, 0, 0,
        /* Masks and shifts */
        0x0000FFFF, 0, 0, 0,
        0, 0, 0, 0
        },
    //-----------------------------------------------------------------------
        { "PF_RG16",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_INTEGER | PFF_NORMALIZED,
        /* Component type and count */
        PCT_SHORT, 2,
        /* rbits, gbits, bbits, abits */
        16, 16, 0, 0,
        /* Masks and shifts */
        0x0000FFFF, 0xFFFF0000, 0, 0,
        0, 16, 0, 0
        },
    //-----------------------------------------------------------------------
        { "PF_RGB16",
        /* Bytes per element */
        6,
        /* Flags */
        PFF_INTEGER | PFF_NORMALIZED,
        /* Component type and count */
        PCT_SHORT, 3,
        /* rbits, gbits, bbits, abits */
        16, 16, 16, 0,
        /* Masks and shifts */
        0x0000FFFF, 0xFFFF0000, 0x0000FFFF, 0,
        0, 16, 0, 0
        },
    //-----------------------------------------------------------------------
        { "PF_RGBA16",
        /* Bytes per element */
        8,
        /* Flags */
        PFF_INTEGER | PFF_NORMALIZED | PFF_HASALPHA,
        /* Component type and count */
        PCT_SHORT, 4,
        /* rbits, gbits, bbits, abits */
        16, 16, 16, 16,
        /* Masks and shifts */
        0x0000FFFF, 0xFFFF0000, 0x0000FFFF, 0xFFFF0000,
        0, 16, 0, 16
        },
    };

    static inline const PixelFormatDescription &GetDescriptionFor(const PixelFormat fmt)
    {
        const int ord = (int)fmt;
        assert(ord >= 0 && ord < PF_COUNT);

        return _pixelFormats[ord];
    }

    UINT32 PixelUtil::GetNumElemBytes(PixelFormat format)
    {
        return GetDescriptionFor(format).elemBytes;
    }

    /**	Handles compression output from NVTT library for a single image. */
    struct NVTTCompressOutputHandler : public nvtt::OutputHandler
    {
        NVTTCompressOutputHandler(UINT8* buffer, UINT32 sizeBytes)
            :buffer(buffer), bufferWritePos(buffer), bufferEnd(buffer + sizeBytes)
        { }

        void beginImage(int size, int width, int height, int depth, int face, int miplevel) override
        { }

        bool writeData(const void* data, int size) override
        {
            assert((bufferWritePos + size) <= bufferEnd);
            memcpy(bufferWritePos, data, size);
            bufferWritePos += size;

            return true;
        }

        void endImage() override
        { }

        UINT8* buffer;
        UINT8* bufferWritePos;
        UINT8* bufferEnd;
    };

    /**	Handles output from NVTT library for a mip-map chain. */
    struct NVTTMipmapOutputHandler : public nvtt::OutputHandler
    {
        NVTTMipmapOutputHandler(const Vector<SPtr<PixelData>>& buffers)
            :buffers(buffers), bufferWritePos(nullptr), bufferEnd(nullptr)
        { }

        void beginImage(int size, int width, int height, int depth, int face, int miplevel) override
        {
            assert(miplevel >= 0 && miplevel < (int)buffers.size());
            assert((UINT32)size == buffers[miplevel]->GetConsecutiveSize());

            activeBuffer = buffers[miplevel];

            bufferWritePos = activeBuffer->GetData();
            bufferEnd = bufferWritePos + activeBuffer->GetConsecutiveSize();
        }

        bool writeData(const void* data, int size) override
        {
            assert((bufferWritePos + size) <= bufferEnd);
            memcpy(bufferWritePos, data, size);
            bufferWritePos += size;

            return true;
        }

        void endImage() override
        { }

        Vector<SPtr<PixelData>> buffers;
        SPtr<PixelData> activeBuffer;

        UINT8* bufferWritePos;
        UINT8* bufferEnd;
    };

    nvtt::Format ToNVTTFormat(PixelFormat format)
    {
        switch (format)
        {
        case PF_BC1:
            return nvtt::Format_BC1;
        case PF_BC1a:
            return nvtt::Format_BC1a;
        case PF_BC2:
            return nvtt::Format_BC2;
        case PF_BC3:
            return nvtt::Format_BC3;
        case PF_BC4:
            return nvtt::Format_BC4;
        case PF_BC5:
            return nvtt::Format_BC5;
        case PF_BC6H:
            return nvtt::Format_BC6;
        case PF_BC7:
            return nvtt::Format_BC7;
        default: // Unsupported format
            return nvtt::Format_BC3;
        }
    }

    nvtt::Quality ToNVTTQuality(CompressionQuality quality)
    {
        switch (quality)
        {
        case CompressionQuality::Fastest:
            return nvtt::Quality_Fastest;
        case CompressionQuality::Highest:
            return nvtt::Quality_Highest;
        case CompressionQuality::Normal:
            return nvtt::Quality_Normal;
        case CompressionQuality::Production:
            return nvtt::Quality_Normal;
        }

        // Unknown quality level
        return nvtt::Quality_Normal;
    }

    nvtt::AlphaMode ToNVTTAlphaMode(AlphaMode alphaMode)
    {
        switch (alphaMode)
        {
        case AlphaMode::None:
            return nvtt::AlphaMode_None;
        case AlphaMode::Premultiplied:
            return nvtt::AlphaMode_Premultiplied;
        case AlphaMode::Transparency:
            return nvtt::AlphaMode_Transparency;
        }

        // Unknown alpha mode
        return nvtt::AlphaMode_None;
    }

    nvtt::WrapMode ToNVTTWrapMode(MipMapWrapMode wrapMode)
    {
        switch (wrapMode)
        {
        case MipMapWrapMode::Clamp:
            return nvtt::WrapMode_Clamp;
        case MipMapWrapMode::Mirror:
            return nvtt::WrapMode_Mirror;
        case MipMapWrapMode::Repeat:
            return nvtt::WrapMode_Repeat;
        }

        // Unknown wrap mode
        return nvtt::WrapMode_Mirror;
    }

    nvtt::MipmapFilter ToNVTTFilter(MipMapFilter filter)
    {
        switch (filter)
        {
        case MipMapFilter::Box:
            return nvtt::MipmapFilter::MipmapFilter_Box;
        case MipMapFilter::Triangle:
            return nvtt::MipmapFilter::MipmapFilter_Triangle;
        case MipMapFilter::Kaiser:
            return nvtt::MipmapFilter::MipmapFilter_Kaiser;
        }

        // Unknown filter mode
        return nvtt::MipmapFilter::MipmapFilter_Kaiser;
    }

    nvtt::RoundMode ToNVTTRoundMode(MipMapRoundMode round)
    {
        switch (round)
        {
        case MipMapRoundMode::RoundNone:
            return nvtt::RoundMode::RoundMode_None;
        case MipMapRoundMode::ToPreviousPowerOfTwo:
            return nvtt::RoundMode::RoundMode_ToPreviousPowerOfTwo;
        case MipMapRoundMode::ToNextPowerOfTwo:
            return nvtt::RoundMode::RoundMode_ToNextPowerOfTwo;
        case MipMapRoundMode::ToNearestPowerOfTwo:
            return nvtt::RoundMode::RoundMode_ToNearestPowerOfTwo;
        }

        // Unknown round mode
        return nvtt::RoundMode::RoundMode_None;
    }

    UINT32 PixelUtil::GetBlockSize(PixelFormat format)
    {
        switch (format)
        {
        case PF_BC1:
        case PF_BC1a:
        case PF_BC4:
            return 8;
        case PF_BC2:
        case PF_BC3:
        case PF_BC5:
        case PF_BC6H:
        case PF_BC7:
            return 16;
        default:
            return GetNumElemBytes(format);
        }
    }

    Vector2I PixelUtil::GetBlockDimensions(PixelFormat format)
    {
        switch (format)
        {
        case PF_BC1:
        case PF_BC1a:
        case PF_BC4:
        case PF_BC2:
        case PF_BC3:
        case PF_BC5:
        case PF_BC6H:
        case PF_BC7:
            return Vector2I(4, 4);
        default:
            return Vector2I(1, 1);
        }
    }

    UINT32 PixelUtil::GetMemorySize(UINT32 width, UINT32 height, UINT32 depth, PixelFormat format)
    {
        if(IsCompressed(format))
        {
            switch(format)
            {
                // BC formats work by dividing the image into 4x4 blocks
                case PF_BC1:
                case PF_BC1a:
                case PF_BC4:
                case PF_BC2:
                case PF_BC3:
                case PF_BC5:
                case PF_BC6H:
                case PF_BC7:
                    width = Math::DivideAndRoundUp(width, 4U);
                    height = Math::DivideAndRoundUp(height, 4U);
                    break;
                default:
                    break;
            }
        }

        return width*height*depth*GetBlockSize(format);
    }

    void PixelUtil::GetPitch(UINT32 width, UINT32 height, UINT32 depth, PixelFormat format,
                         UINT32& rowPitch, UINT32& depthPitch)
    {
        UINT32 blockSize = GetBlockSize(format);

        if (IsCompressed(format))
        {
            switch (format)
            {
                // BC formats work by dividing the image into 4x4 blocks
            case PF_BC1:
            case PF_BC1a:
            case PF_BC4:
            case PF_BC2:
            case PF_BC3:
            case PF_BC5:
            case PF_BC6H:
            case PF_BC7:
                width = Math::DivideAndRoundUp(width, 4U);
                height = Math::DivideAndRoundUp(height, 4U);
                break;
            default:
                break;
            }
        }

        rowPitch = width * blockSize;
        depthPitch = width * height * blockSize;
    }

    void PixelUtil::GetSizeForMipLevel(UINT32 width, UINT32 height, UINT32 depth, UINT32 mipLevel,
        UINT32& mipWidth, UINT32& mipHeight, UINT32& mipDepth)
    {
        mipWidth = width;
        mipHeight = height;
        mipDepth = depth;

        for (UINT32 i = 0; i < mipLevel; i++)
        {
            if (mipWidth != 1) mipWidth /= 2;
            if (mipHeight != 1) mipHeight /= 2;
            if (mipDepth != 1) mipDepth /= 2;
        }
    }

    UINT32 PixelUtil::GetNumElemBits(PixelFormat format)
    {
        return GetDescriptionFor(format).elemBytes * 8;
    }

    bool PixelUtil::HasAlpha(PixelFormat format)
    {
        return (PixelUtil::GetFlags(format) & PFF_HASALPHA) > 0;
    }

    bool PixelUtil::IsFloatingPoint(PixelFormat format)
    {
        return (PixelUtil::GetFlags(format) & PFF_FLOAT) > 0;
    }

    UINT32 PixelUtil::GetFlags(PixelFormat format)
    {
        return GetDescriptionFor(format).flags;
    }

    bool PixelUtil::IsDepth(PixelFormat format)
    {
        return (PixelUtil::GetFlags(format) & PFF_DEPTH) > 0;
    }

    bool PixelUtil::IsCompressed(PixelFormat format)
    {
        return (PixelUtil::GetFlags(format) & PFF_COMPRESSED) > 0;
    }

    bool PixelUtil::CheckFormat(PixelFormat& format, TextureType texType, int usage)
    {
        // First check just the usage since it's the most limiting factor

        //// Depth-stencil only supports depth formats
        if ((usage & TU_DEPTHSTENCIL) != 0)
        {
            if (IsDepth(format))
                return true;

            format = PF_D32_S8X24;
            return false;
        }

        //// Render ta.Gets support everything but compressed & depth-stencil formats
        if ((usage & TU_RENDERTARGET) != 0)
        {
            if (!IsDepth(format) && !IsCompressed(format))
                return true;

            format = PF_RGBA8;
            return false;
        }

        //// Load-store textures support everything but compressed & depth-stencil formats
        if ((usage & TU_LOADSTORE) != 0)
        {
            if (!IsDepth(format) && !IsCompressed(format))
                return true;

            format = PF_RGBA8;
            return false;
        }

        //// Sampled texture support depends on texture type
        switch (texType)
        {
        case TEX_TYPE_1D:
        {
            // 1D textures support anything but depth & compressed formats
            if (!IsDepth(format) && !IsCompressed(format))
                return true;

            format = PF_RGBA8;
            return false;
        }
        case TEX_TYPE_3D:
        {
            // 3D textures support anything but depth & compressed formats
            if (!IsDepth(format))
                return true;

            format = PF_RGBA8;
            return false;
        }
        default: // 2D & cube
        {
            // 2D/cube textures support anything but depth formats
            if (!IsDepth(format))
                return true;

            format = PF_RGBA8;
            return false;
        }
        }
    }

    bool PixelUtil::IsValidExtent(UINT32 width, UINT32 height, UINT32 depth, PixelFormat format)
    {
        if(IsCompressed(format))
        {
            switch(format)
            {
                case PF_BC1:
                case PF_BC2:
                case PF_BC1a:
                case PF_BC3:
                case PF_BC4:
                case PF_BC5:
                case PF_BC6H:
                case PF_BC7:
                    return ((width & 3) == 0 && (height & 3) == 0 && depth == 1);
                default:
                    return true;
            }
        }
        else
        {
            return true;
        }
    }

    void PixelUtil::GetBitDepths(PixelFormat format, int(&rgba)[4])
    {
        const PixelFormatDescription& des = GetDescriptionFor(format);
        rgba[0] = des.rbits;
        rgba[1] = des.gbits;
        rgba[2] = des.bbits;
        rgba[3] = des.abits;
    }

    void PixelUtil::GetBitMasks(PixelFormat format, UINT32(&rgba)[4])
    {
        const PixelFormatDescription& des = GetDescriptionFor(format);
        rgba[0] = des.rmask;
        rgba[1] = des.gmask;
        rgba[2] = des.bmask;
        rgba[3] = des.amask;
    }

    void PixelUtil::GetBitShifts(PixelFormat format, UINT8(&rgba)[4])
    {
        const PixelFormatDescription& des = GetDescriptionFor(format);
        rgba[0] = des.rshift;
        rgba[1] = des.gshift;
        rgba[2] = des.bshift;
        rgba[3] = des.ashift;
    }

    String PixelUtil::GetFormatName(PixelFormat srcformat)
    {
        return GetDescriptionFor(srcformat).name;
    }

    bool PixelUtil::IsAccessible(PixelFormat srcformat)
    {
        if (srcformat == PF_UNKNOWN)
            return false;

        UINT32 flags = GetFlags(srcformat);
        return !((flags & PFF_COMPRESSED) || (flags & PFF_DEPTH));
    }

    PixelComponentType PixelUtil::GetElementType(PixelFormat format)
    {
        const PixelFormatDescription& des = GetDescriptionFor(format);
        return des.componentType;
    }

    UINT32 PixelUtil::GetNumElements(PixelFormat format)
    {
        const PixelFormatDescription& des = GetDescriptionFor(format);
        return des.componentCount;
    }

    UINT32 PixelUtil::GetMaxMipmaps(UINT32 width, UINT32 height, UINT32 depth)
    {
        UINT32 count = 0;
        if ((width > 0) && (height > 0))
        {
            while (!(width == 1 && height == 1 && depth == 1))
            {
                if (width > 1)		width = width / 2;
                if (height > 1)	    height = height / 2;
                if (depth > 1)		depth = depth / 2;

                count++;
            }
        }

        return count;
    }

    void PixelUtil::PackColor(const Color& color, PixelFormat format, void* dest)
    {
        PackColor(color.r, color.g, color.b, color.a, format, dest);
    }

    void PixelUtil::PackColor(UINT8 r, UINT8 g, UINT8 b, UINT8 a, PixelFormat format, void* dest)
    {
        const PixelFormatDescription &des = GetDescriptionFor(format);

        if (des.flags & PFF_INTEGER)
        {
            // Shortcut for integer formats packing
            UINT32 value = ((Bitwise::FixedToFixed(r, 8, des.rbits) << des.rshift) & des.rmask) |
                ((Bitwise::FixedToFixed(g, 8, des.gbits) << des.gshift) & des.gmask) |
                ((Bitwise::FixedToFixed(b, 8, des.bbits) << des.bshift) & des.bmask) |
                ((Bitwise::FixedToFixed(a, 8, des.abits) << des.ashift) & des.amask);

            // And write to memory
            Bitwise::IntWrite(dest, des.elemBytes, value);
        }
        else
        {
            // Convert to float
            PackColor((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f, format, dest);
        }
    }

    void PixelUtil::PackColor(float r, float g, float b, float a, const PixelFormat format, void* dest)
    {
        // Special cases
        if (format == PF_RG11B10F)
        {
            UINT32 value;
            value = Bitwise::FloatToFloat11(r);
            value |= Bitwise::FloatToFloat11(g) << 11;
            value |= Bitwise::FloatToFloat10(b) << 22;

            ((UINT32*)dest)[0] = value;
            return;
        }

        if (format == PF_RGB10A2)
        {
            TE_DEBUG("packColor() not implemented for format \"" + GetFormatName(PF_RGB10A2) + "\"");
            return;
        }

        // All other formats handled in a generic way
        const PixelFormatDescription& des = GetDescriptionFor(format);
        assert(des.componentCount <= 4);

        float inputs[] = { r, g, b, a };
        UINT8 bits[] = { des.rbits, des.gbits, des.bbits, des.abits };
        UINT32 masks[] = { des.rmask, des.gmask, des.bmask, des.amask };
        UINT8 shifts[] = { des.rshift, des.gshift, des.bshift, des.ashift };

        memset(dest, 0, des.elemBytes);

        UINT32 curBit = 0;
        UINT32 prevDword = 0;
        UINT32 dwordValue = 0;
        for (UINT32 i = 0; i < des.componentCount; i++)
        {
            UINT32 curDword = curBit / 32;

            // New dword reached, write current one and advance
            if(curDword > prevDword)
            {
                UINT32* curDst = ((UINT32*)dest) + prevDword;
                Bitwise::IntWrite(curDst, 4, dwordValue);

                dwordValue = 0;
                prevDword = curDword;
            }

            if (des.flags & PFF_INTEGER)
            {
                if (des.flags & PFF_NORMALIZED)
                {
                    if (des.flags & PFF_SIGNED)
                        dwordValue |= (Bitwise::SnormToUint(inputs[i], bits[i]) << shifts[i]) & masks[i];
                    else
                        dwordValue |= (Bitwise::UnormToUint(inputs[i], bits[i]) << shifts[i]) & masks[i];
                }
                else
                {
                    // Note: Casting integer to float. A better option would be to have a separate unpackColor that has
                    // integer output parameters.
                    dwordValue |= (((UINT32)inputs[i]) << shifts[i]) & masks[i];
                }
            }
            else if (des.flags & PFF_FLOAT)
            {
                // Note: Not handling unsigned floats

                if (des.componentType == PCT_FLOAT16)
                    dwordValue |= (Bitwise::FloatToHalf(inputs[i]) << shifts[i]) & masks[i];
                else
                    dwordValue |= *(UINT32*)&inputs[i];
            }
            else
            {
                TE_DEBUG("packColor() not implemented for format \"" + GetFormatName(PF_RGB10A2) + "\"");
                return;
            }

            curBit += bits[i];
        }

        // Write last dword
        UINT32 numBytes = std::min((prevDword + 1) * 4, (UINT32)des.elemBytes) - (prevDword * 4);
        UINT32* curDst = ((UINT32*)dest) + prevDword;
        Bitwise::IntWrite(curDst, numBytes, dwordValue);
    }

    void PixelUtil::UnpackColor(Color* color, PixelFormat format, const void* src)
    {
        UnpackColor(&color->r, &color->g, &color->b, &color->a, format, src);
    }

    void PixelUtil::UnpackColor(UINT8* r, UINT8* g, UINT8* b, UINT8* a, PixelFormat format, const void* src)
    {
        const PixelFormatDescription &des = GetDescriptionFor(format);

        if (des.flags & PFF_INTEGER)
        {
            // Shortcut for integer formats unpacking
            const UINT32 value = Bitwise::IntRead(src, des.elemBytes);

            *r = (UINT8)Bitwise::FixedToFixed((value & des.rmask) >> des.rshift, des.rbits, 8);
            *g = (UINT8)Bitwise::FixedToFixed((value & des.gmask) >> des.gshift, des.gbits, 8);
            *b = (UINT8)Bitwise::FixedToFixed((value & des.bmask) >> des.bshift, des.bbits, 8);

            if (des.flags & PFF_HASALPHA)
            {
                *a = (UINT8)Bitwise::FixedToFixed((value & des.amask) >> des.ashift, des.abits, 8);
            }
            else
            {
                *a = 255; // No alpha, default a component to full
            }
        }
        else
        {
            // Do the operation with the more generic floating point
            float rr, gg, bb, aa;
            UnpackColor(&rr, &gg, &bb, &aa, format, src);

            *r = (UINT8)Bitwise::UnormToUint(rr, 8);
            *g = (UINT8)Bitwise::UnormToUint(gg, 8);
            *b = (UINT8)Bitwise::UnormToUint(bb, 8);
            *a = (UINT8)Bitwise::UnormToUint(aa, 8);
        }
    }

    void PixelUtil::UnpackColor(float* r, float* g, float* b, float* a, PixelFormat format, const void* src)
    {
        // Special cases
        if(format == PF_RG11B10F)
        {
            UINT32 value = ((UINT32*)src)[0];
            *r = Bitwise::Float11ToFloat(value);
            *g = Bitwise::Float11ToFloat(value >> 11);
            *b = Bitwise::Float10ToFloat(value >> 22);

            return;
        }

        if(format == PF_RGB10A2)
        {
            TE_DEBUG("unpackColor() not implemented for format \"" + GetFormatName(PF_RGB10A2) + "\"");
            return;
        }

        // All other formats handled in a generic way
        const PixelFormatDescription& des = GetDescriptionFor(format);
        assert(des.componentCount <= 4);

        float* outputs[] = { r, g, b, a };
        UINT8 bits[] = { des.rbits, des.gbits, des.bbits, des.abits };
        UINT32 masks[] = { des.rmask, des.gmask, des.bmask, des.amask };
        UINT8 shifts[] = { des.rshift, des.gshift, des.bshift, des.ashift };

        UINT32 curBit = 0;
        for(UINT32 i = 0; i < des.componentCount; i++)
        {
            UINT32 curDword = curBit / 32;
            UINT32 numBytes = std::min((curDword + 1) * 4, (UINT32)des.elemBytes) - (curDword * 4);

            UINT32* curSrc = ((UINT32*)src) + curDword;
            UINT32 value = Bitwise::IntRead(curSrc, numBytes);
            if(des.flags & PFF_INTEGER)
            {
                if(des.flags & PFF_NORMALIZED)
                {
                    if (des.flags & PFF_SIGNED)
                        *outputs[i] = Bitwise::UintToSnorm((value & masks[i]) >> shifts[i], bits[i]);
                    else
                        *outputs[i] = Bitwise::UintToUnorm((value & masks[i]) >> shifts[i], bits[i]);
                }
                else
                {
                    // Note: Casting integer to float. A better option would be to have a separate unpackColor that has
                    // integer output parameters.
                    *outputs[i] = (float)((value & masks[i]) >> shifts[i]);
                }
            }
            else if(des.flags & PFF_FLOAT)
            {
                // Note: Not handling unsigned floats

                if (des.componentType == PCT_FLOAT16)
                    *outputs[i] = Bitwise::HalfToFloat((UINT16)((value & masks[i]) >> shifts[i]));
                else
                    *outputs[i] = *(float*)&value;
            }
            else
            {
                TE_DEBUG("unpackColor() not implemented for format \"" + GetFormatName(format) + "\"");
                return;
            }

            curBit += bits[i];
        }

        // Fill empty components
        for (UINT32 i = des.componentCount; i < 3; i++)
            *outputs[i] = 0.0f;

        if (des.componentCount < 4)
            *outputs[3] = 1.0f;
    }

    void PixelUtil::PackDepth(float depth, const PixelFormat format, void* dest)
    {
        if (!IsDepth(format))
        {
            TE_DEBUG("Cannot convert depth to {0}: it is not a depth format \"" + GetFormatName(format) + "\"");
            return;
        }

        TE_DEBUG("Method is not implemented");
        // TODO implement depth packing
    }

    float PixelUtil::UnpackDepth(PixelFormat format, void* src)
    {
        if (!IsDepth(format))
        {
            TE_DEBUG("Cannot unpack from {0}: it is not a depth format \"" + GetFormatName(format) + "\"");
            return 0;
        }

        UINT32* color = (UINT32 *)src;
        UINT32 masked = 0;
        switch (format)
        {
        case PF_D24S8:
            return  static_cast<float>(*color & 0x00FFFFFF) / (float)16777216;
            break;
        case PF_D16:
            return static_cast<float>(*color & 0xFFFF) / (float)65536;
            break;
        case PF_D32:
            masked = *color & 0xFFFFFFFF;
            return *((float*)&masked);
            break;
        case PF_D32_S8X24:
            masked = *color & 0xFFFFFFFF;
            return *( (float*) &masked );
            break;
        default:
            TE_DEBUG("Cannot unpack from \"" + GetFormatName(format) + "\"");
            return 0;
            break;
        }
    }

    void PixelUtil::BulkPixelConversion(const PixelData &src, PixelData &dst)
    {
        if(src.GetWidth() != dst.GetWidth() || src.GetHeight() != dst.GetHeight() || src.GetDepth() != dst.GetDepth())
        {
            TE_DEBUG("Cannot convert pixels between buffers of different sizes.");
            return;
        }

        // The easy case
        if (src.GetFormat() == dst.GetFormat())
        {
            // Everything consecutive?
            if (src.IsConsecutive() && dst.IsConsecutive())
            {
                memcpy(dst.GetData(), src.GetData(), src.GetConsecutiveSize());
                return;
            }

            PixelFormat format = src.GetFormat();
            UINT32 pixelSize = GetNumElemBytes(format);

            Vector2I blockDim = GetBlockDimensions(format);
            if(IsCompressed(format))
            {
                UINT32 blockSize = GetBlockSize(format);
                pixelSize = blockSize / blockDim.x;

                if(src.GetLeft() % blockDim.x != 0 || src.GetTop() % blockDim.y != 0)
                {
                    TE_DEBUG("Source offset must be a multiple of block size for compressed formats.");
                }

                if(dst.GetLeft() % blockDim.x != 0 || dst.GetTop() % blockDim.y != 0)
                {
                    TE_DEBUG("Destination offset must be a multiple of block size for compressed formats.");
                }
            }

            UINT8* srcPtr = static_cast<UINT8*>(src.GetData())
                + src.GetLeft() * pixelSize + src.GetTop() * src.GetRowPitch() + src.GetFront() * src.GetSlicePitch();
            UINT8* dstPtr = static_cast<UINT8*>(dst.GetData())
                + dst.GetLeft() * pixelSize + dst.GetTop() * dst.GetRowPitch() + dst.GetFront() * dst.GetSlicePitch();

            // Get pitches+skips in bytes
            const UINT32 srcRowPitchBytes = src.GetRowPitch();
            const UINT32 srcSliceSkipBytes = src.GetSliceSkip();

            const UINT32 dstRowPitchBytes = dst.GetRowPitch();
            const UINT32 dstSliceSkipBytes = dst.GetSliceSkip();

            // Otherwise, copy per row
            const UINT32 rowSize = src.GetWidth()*pixelSize;
            for (UINT32 z = src.GetFront(); z < src.GetBack(); z++)
            {
                for (UINT32 y = src.GetTop(); y < src.GetBottom(); y += blockDim.y)
                {
                    memcpy(dstPtr, srcPtr, rowSize);

                    srcPtr += srcRowPitchBytes;
                    dstPtr += dstRowPitchBytes;
                }

                srcPtr += srcSliceSkipBytes;
                dstPtr += dstSliceSkipBytes;
            }

            return;
        }

        // Check for compressed formats, we don't support decompression
        if (IsCompressed(src.GetFormat()))
        {
            if (src.GetFormat() != dst.GetFormat())
            {
                TE_DEBUG("Cannot convert from a compressed format to another format.");
                return;
            }
        }

        // Check for compression
        if (IsCompressed(dst.GetFormat()))
        {
            if (src.GetFormat() != dst.GetFormat())
            {
                CompressionOptions co;
                co.Format = dst.GetFormat();
                Compress(src, dst, co);

                return;
            }
        }

        UINT32 srcPixelSize = GetNumElemBytes(src.GetFormat());
        UINT32 dstPixelSize = GetNumElemBytes(dst.GetFormat());
        UINT8 *srcptr = static_cast<UINT8*>(src.GetData())
            + src.GetLeft() * srcPixelSize + src.GetTop() * src.GetRowPitch() + src.GetFront() * src.GetSlicePitch();
        UINT8 *dstptr = static_cast<UINT8*>(dst.GetData())
            + dst.GetLeft() * dstPixelSize + dst.GetTop() * dst.GetRowPitch() + dst.GetFront() * dst.GetSlicePitch();

        // Get pitches+skips in bytes
        UINT32 srcRowSkipBytes = src.GetRowSkip();
        UINT32 srcSliceSkipBytes = src.GetSliceSkip();
        UINT32 dstRowSkipBytes = dst.GetRowSkip();
        UINT32 dstSliceSkipBytes = dst.GetSliceSkip();

        // The brute force fallback
        float r, g, b, a;
        for (UINT32 z = src.GetFront(); z < src.GetBack(); z++)
        {
            for (UINT32 y = src.GetTop(); y < src.GetBottom(); y++)
            {
                for (UINT32 x = src.GetLeft(); x < src.GetRight(); x++)
                {
                    UnpackColor(&r, &g, &b, &a, src.GetFormat(), srcptr);
                    PackColor(r, g, b, a, dst.GetFormat(), dstptr);

                    srcptr += srcPixelSize;
                    dstptr += dstPixelSize;
                }

                srcptr += srcRowSkipBytes;
                dstptr += dstRowSkipBytes;
            }

            srcptr += srcSliceSkipBytes;
            dstptr += dstSliceSkipBytes;
        }
    }

    void PixelUtil::FlipComponentOrder(PixelData& data)
    {
        if (IsCompressed(data.GetFormat()))
        {
            TE_DEBUG("FlipComponentOrder() not supported on compressed images.");
            return;
        }

        const PixelFormatDescription& pfd = GetDescriptionFor(data.GetFormat());
        if(pfd.elemBytes > 4)
        {
            TE_DEBUG("FlipComponentOrder() only supported on 4 byte or smaller pixel formats.");
            return;
        }

        if (pfd.componentCount <= 1) // Nothing to flip
            return;

        bool bitCountMismatch = false;
        if (pfd.rbits != pfd.gbits)
            bitCountMismatch = true;

        if(pfd.componentCount > 2 && pfd.rbits != pfd.bbits)
            bitCountMismatch = true;

        if (pfd.componentCount > 3 && pfd.rbits != pfd.abits)
            bitCountMismatch = true;

        if(bitCountMismatch)
        {
            TE_DEBUG("flipComponentOrder() not supported for formats that don't have the same number of bytes for all components.");
            return;
        }

        struct CompData
        {
            UINT32 mask;
            UINT8 shift;
        };

        std::array<CompData, 4> compData =
        {{
            { pfd.rmask, pfd.rshift },
            { pfd.gmask, pfd.gshift },
            { pfd.bmask, pfd.bshift },
            { pfd.amask, pfd.ashift }
        }};

        // Ensure unused components are at the end, after sort
        if (pfd.componentCount < 4)
            compData[3].shift = 0xFF;

        if (pfd.componentCount < 3)
            compData[2].shift = 0xFF;

        std::sort(compData.begin(), compData.end(),
            [&](const CompData& lhs, const CompData& rhs) { return lhs.shift < rhs.shift; }
        );

        UINT8* dataPtr = data.GetData();

        UINT32 pixelSize = pfd.elemBytes;
        UINT32 rowSkipBytes = data.GetRowSkip();
        UINT32 sliceSkipBytes = data.GetSliceSkip();

        for (UINT32 z = 0; z < data.GetDepth(); z++)
        {
            for (UINT32 y = 0; y < data.GetHeight(); y++)
            {
                for (UINT32 x = 0; x < data.GetWidth(); x++)
                {
                    if(pfd.componentCount == 2)
                    {
                        UINT64 pixelData = 0;
                        memcpy(&pixelData, dataPtr, pixelSize);

                        UINT64 output = 0;
                        output |= (pixelData & compData[1].mask) >> compData[1].shift;
                        output |= (pixelData & compData[0].mask) << compData[1].shift;

                        memcpy(dataPtr, &output, pixelSize);
                    }
                    else if(pfd.componentCount == 3)
                    {
                        UINT64 pixelData = 0;
                        memcpy(&pixelData, dataPtr, pixelSize);

                        UINT64 output = 0;
                        output |= (pixelData & compData[2].mask) >> compData[2].shift;
                        output |= (pixelData & compData[0].mask) << compData[2].shift;

                        memcpy(dataPtr, &output, pixelSize);
                    }
                    else if(pfd.componentCount == 4)
                    {
                        UINT64 pixelData = 0;
                        memcpy(&pixelData, dataPtr, pixelSize);

                        UINT64 output = 0;
                        output |= (pixelData & compData[3].mask) >> compData[3].shift;
                        output |= (pixelData & compData[0].mask) << compData[3].shift;

                        output |= (pixelData & compData[2].mask) >> (compData[2].shift - compData[1].shift);
                        output |= (pixelData & compData[1].mask) << (compData[2].shift - compData[1].shift);

                        memcpy(dataPtr, &output, pixelSize);
                    }

                    dataPtr += pixelSize;
                }

                dataPtr += rowSkipBytes;
            }

            dataPtr += sliceSkipBytes;
        }
    }

    void PixelUtil::Compress(const PixelData& src, PixelData& dst, const CompressionOptions& options)
    {
        if (!IsCompressed(options.Format))
        {
            TE_DEBUG("Compression failed. Destination format is not a valid compressed format.");
            return;
        }

        if (src.GetDepth() != 1)
        {
            TE_DEBUG("Compression failed. 3D texture compression not supported.");
            return;
        }

        if (IsCompressed(src.GetFormat()))
        {
            TE_DEBUG("Compression failed. Source data cannot be compressed.");
            return;
        }

        PixelFormat interimFormat = options.Format == PF_BC6H ? PF_RGBA32F : PF_BGRA8;

        PixelData interimData(src.GetWidth(), src.GetHeight(), 1, interimFormat);
        interimData.AllocateInternalBuffer();
        BulkPixelConversion(src, interimData);

        nvtt::InputOptions io;
        io.setTextureLayout(nvtt::TextureType_2D, src.GetWidth(), src.GetHeight());
        io.setMipmapGeneration(false);
        io.setAlphaMode(ToNVTTAlphaMode(options.Alpha));
        io.setNormalMap(options.IsNormalMap);
        io.setRoundMode(nvtt::RoundMode::RoundMode_ToNearestPowerOfTwo);

        if (interimFormat == PF_RGBA32F)
            io.setFormat(nvtt::InputFormat_RGBA_32F);
        else
            io.setFormat(nvtt::InputFormat_BGRA_8UB);

        if (options.IsSRGB)
            io.setGamma(2.2f, 2.2f);
        else
            io.setGamma(1.0f, 1.0f);

        io.setMipmapData(interimData.GetData(), src.GetWidth(), src.GetHeight());

        nvtt::CompressionOptions co;
        co.setFormat(ToNVTTFormat(options.Format));
        co.setQuality(ToNVTTQuality(options.Quality));

        NVTTCompressOutputHandler outputHandler(dst.GetData(), dst.GetConsecutiveSize());

        nvtt::OutputOptions oo;
        oo.setOutputHeader(false);
        oo.setOutputHandler(&outputHandler);

        nvtt::Compressor compressor;
        if (!compressor.process(io, co, oo))
        {
            TE_DEBUG("Compression failed. Internal error.");
            return;
        }
    }

    Vector<SPtr<PixelData>> PixelUtil::GenMipmaps(const PixelData& src, const MipMapGenOptions& options, UINT32 maxMip)
    {
        Vector<SPtr<PixelData>> outputMipBuffers;

        if (src.GetDepth() != 1)
        {
            TE_DEBUG("Mipmap generation failed. 3D texture formats not supported.");
            return outputMipBuffers;
        }

        if (IsCompressed(src.GetFormat()))
        {
            TE_DEBUG("Mipmap generation failed. Source data cannot be compressed.");
            return outputMipBuffers;
        }

        PixelFormat interimFormat = IsFloatingPoint(src.GetFormat()) ? PF_RGBA32F : PF_BGRA8;

        PixelData interimData(src.GetWidth(), src.GetHeight(), 1, interimFormat);
        interimData.AllocateInternalBuffer();
        BulkPixelConversion(src, interimData);

        if (interimFormat != PF_RGBA32F)
            FlipComponentOrder(interimData);

        UINT32 numMips = GetMaxMipmaps(src.GetWidth(), src.GetHeight(), 1);
        if (maxMip > 0)
            numMips = std::min(numMips, maxMip);

        nvtt::InputOptions io;
        io.setTextureLayout(nvtt::TextureType_2D, src.GetWidth(), src.GetHeight());
        io.setMipmapGeneration(true, numMips);
        io.setNormalMap(options.IsNormalMap);
        io.setNormalizeMipmaps(options.NormalizeMipmaps);
        io.setWrapMode(ToNVTTWrapMode(options.WrapMode));
        io.setAlphaMode(ToNVTTAlphaMode(options.Alpha));
        io.setMipmapFilter(ToNVTTFilter(options.Filter));
        io.setMipmapData(interimData.GetData(), src.GetWidth(), src.GetHeight());

        if (interimFormat == PF_RGBA32F) io.setFormat(nvtt::InputFormat_RGBA_32F);
        else io.setFormat(nvtt::InputFormat_BGRA_8UB);

        if (options.IsSRGB) io.setGamma(2.2f, 2.2f);
        else io.setGamma(1.0f, 1.0f);

        nvtt::CompressionOptions co;
        co.setFormat(nvtt::Format_RGBA);
        co.setQuality(ToNVTTQuality(options.Quality));

        if (interimFormat == PF_RGBA32F)
        {
            co.setPixelType(nvtt::PixelType_Float);
            co.setPixelFormat(32, 32, 32, 32);
        }
        else
        {
            co.setPixelType(nvtt::PixelType_UnsignedNorm);
            co.setPixelFormat(32, 0x0000FF00, 0x00FF0000, 0xFF000000, 0x000000FF);
        }

        Vector<SPtr<PixelData>> rgbaMipBuffers;

        // Note: This can be done more effectively without creating so many temp buffers
        // and working with the original formats directly, but it would complicate the code
        // too much at the moment.
        UINT32 curWidth = src.GetWidth();
        UINT32 curHeight = src.GetHeight();

        if (options.RoundMode != MipMapRoundMode::RoundNone)
        {
            if (!Bitwise::IsPow2(curWidth) || !Bitwise::IsPow2(curHeight))
            {
                if (options.RoundMode == MipMapRoundMode::ToPreviousPowerOfTwo)
                {
                    curWidth = Math::ToPreviousPowerOf2(curWidth);
                    curHeight = Math::ToPreviousPowerOf2(curHeight);
                }

                if (options.RoundMode == MipMapRoundMode::ToNextPowerOfTwo)
                {
                    curWidth = Math::ToNextPowerOf2(curWidth);
                    curHeight = Math::ToNextPowerOf2(curHeight);
                }
            }

            numMips = GetMaxMipmaps(curWidth, curHeight, 1);
            if (maxMip > 0)
                numMips = std::min(numMips, maxMip);

            io.setRoundMode(ToNVTTRoundMode(options.RoundMode));
        }

        io.setMipmapGeneration(true, numMips);

        for (UINT32 i = 0; i < numMips; i++)
        {
            rgbaMipBuffers.push_back(te_shared_ptr_new<PixelData>(curWidth, curHeight, 1, interimFormat));
            rgbaMipBuffers.back()->AllocateInternalBuffer();

            if (curWidth > 1)
                curWidth = curWidth / 2;

            if (curHeight > 1)
                curHeight = curHeight / 2;
        }

        rgbaMipBuffers.push_back(te_shared_ptr_new<PixelData>(curWidth, curHeight, 1, interimFormat));
        rgbaMipBuffers.back()->AllocateInternalBuffer();

        NVTTMipmapOutputHandler outputHandler(rgbaMipBuffers);

        nvtt::OutputOptions oo;
        oo.setOutputHeader(false);
        oo.setOutputHandler(&outputHandler);

        nvtt::Compressor compressor;
        if (!compressor.process(io, co, oo))
        {
            TE_DEBUG("Mipmap generation failed. Internal error.");
            return outputMipBuffers;
        }

        interimData.FreeInternalBuffer();

        for (UINT32 i = 0; i < (UINT32)rgbaMipBuffers.size(); i++)
        {
            SPtr<PixelData> argbBuffer = rgbaMipBuffers[i];
            SPtr<PixelData> outputBuffer = te_shared_ptr_new<PixelData>(argbBuffer->GetWidth(), argbBuffer->GetHeight(), 1, src.GetFormat());
            outputBuffer->AllocateInternalBuffer();

            BulkPixelConversion(*argbBuffer, *outputBuffer);
            argbBuffer->FreeInternalBuffer();

            outputMipBuffers.push_back(outputBuffer);
        }

        return outputMipBuffers;
    }

    SPtr<Texture> PixelUtil::GenMipmaps(const TEXTURE_DESC& desc, const Vector<SPtr<PixelData>>& srcs, const MipMapGenOptions& options, UINT32 maxMip)
    {
        SPtr<Texture> output = nullptr;
        SPtr<Texture> tmp = nullptr;
        bool everythingOk = true;

        if (!RendererMaterialManager::IsStarted())
            return output;

        static TextureDownsampleMat* textureMat = TextureDownsampleMat::Get();
        static TextureCubeDownsampleMat* textureCubeMat = TextureCubeDownsampleMat::Get();

        if (desc.Type != TextureType::TEX_TYPE_2D && desc.Type != TextureType::TEX_TYPE_CUBE_MAP)
        {
            TE_DEBUG("Mipmap generation failed. Only 2D and Cube textures are supported.");
            return output;
        }

        for (auto& src : srcs)
        {
            if (src->GetDepth() != 1)
            {
                TE_DEBUG("Mipmap generation failed. 3D texture formats not supported.");
                return output;
            }

            if (IsCompressed(src->GetFormat()))
            {
                TE_DEBUG("Mipmap generation failed. Source data cannot be compressed.");
                return output;
            }

            if (src->GetWidth() != desc.Width || src->GetHeight() != desc.Height || src->GetDepth() != desc.Depth)
            {
                TE_DEBUG("Output buffer does not correspond to input buffer, mipmap can't be generated with GPU.");
                everythingOk = false;
            }
        }

        if (desc.Type == TextureType::TEX_TYPE_2D && srcs.size() != 1)
        {
            TE_DEBUG("2D Texture must have only one PixelData.");
            return output;
        }

        if (desc.Type == TextureType::TEX_TYPE_CUBE_MAP && srcs.size() != 6)
        {
            TE_DEBUG("Cube Texture must have 6 PixelData.");
            return output;
        }

        RecursiveLock lock(_mutex);

        if(!everythingOk)
        {
            if (desc.NumMips == 0)
            {
                TEXTURE_DESC mipTexDesc = desc;
                mipTexDesc.Width = srcs[0]->GetWidth();
                mipTexDesc.Height = srcs[0]->GetHeight();
                mipTexDesc.Depth = srcs[0]->GetDepth();
                mipTexDesc.NumMips = 0;
                output = Texture::CreatePtr(mipTexDesc);

                for (UINT32 i = 0; i < srcs.size(); i++)
                {
                    SPtr<PixelData> dst = output->GetProperties().AllocBuffer(0, 0);

                    PixelUtil::BulkPixelConversion(*srcs[i], *dst);
                    output->WriteData(*dst, 0, i);
                }
            }

            return output;
        }
        else
        {
            RenderAPI::Instance().PushMarker("[Draw] MipMap", Color(0.78f, 0.65f, 0.21f));

            TEXTURE_DESC mipTexDesc = desc;
            output = Texture::CreatePtr(mipTexDesc);
            mipTexDesc.Usage |= TU_RENDERTARGET;
            tmp = Texture::CreatePtr(mipTexDesc);

            for (UINT32 i = 0; i < srcs.size(); i++)
            {
                SPtr<PixelData> dst = output->GetProperties().AllocBuffer(0, 0);

                PixelUtil::BulkPixelConversion(*srcs[i], *dst);
                output->WriteData(*dst, 0, i);
                tmp->WriteData(*dst, 0, i);
            }

            if (mipTexDesc.NumMips > 0 && everythingOk)
            {
                // TODO PRESERVE ALPHA COVERAGE

                for (UINT32 mip = 1; mip < mipTexDesc.NumMips; mip++)
                {
                    UINT32 srcMip = mip - 1;

                    if (mipTexDesc.Type == TextureType::TEX_TYPE_2D)
                    {
                        RENDER_TEXTURE_DESC mipDesc;
                        mipDesc.ColorSurfaces[0].Tex = tmp;
                        mipDesc.ColorSurfaces[0].MipLevel = mip;
                        mipDesc.ColorSurfaces[0].NumFaces = 1;

                        SPtr<RenderTarget> target = RenderTexture::Create(mipDesc);
                        textureMat->Execute(output, srcMip, target);

                        TEXTURE_COPY_DESC copyDesc;
                        copyDesc.SrcFace = 0;
                        copyDesc.SrcMip = mip;
                        copyDesc.DstFace = 0;
                        copyDesc.DstMip = mip;

                        tmp->Copy(output, copyDesc);
                    }
                    else
                    {
                        // TODO CUBE MIPMAP
                    }
                }
            }

            RenderAPI::Instance().PopMarker();
        }

        return output;
    }

    void PixelUtil::Mirror(PixelData& pixelData, INT32 mode)
    {
        UINT32 width = pixelData.GetWidth();
        UINT32 height = pixelData.GetHeight();
        UINT32 depth = pixelData.GetDepth();

        UINT32 elemSize = GetNumElemBytes(pixelData.GetFormat());

        if (mode & MirrorMode::Z)
        {
            UINT32 sliceSize = width * height * elemSize;
            UINT8* sliceTemp = te_allocate<UINT8>(sliceSize * sizeof(UINT8));

            UINT8* dataPtr = pixelData.GetData();
            UINT32 halfDepth = depth / 2;
            for (UINT32 z = 0; z < halfDepth; z++)
            {
                UINT32 srcZ = z * sliceSize;
                UINT32 dstZ = (depth - z - 1) * sliceSize;

                memcpy(sliceTemp, &dataPtr[dstZ], sliceSize);
                memcpy(&dataPtr[dstZ], &dataPtr[srcZ], sliceSize);
                memcpy(&dataPtr[srcZ], sliceTemp, sliceSize);
            }

            // Note: If flipping Y or X as well I could do it here without an extra set of memcpys

            te_free(sliceTemp);
        }

        if(mode & MirrorMode::Y)
        {
            UINT32 rowSize = width * elemSize;
            UINT8* rowTemp = te_allocate<UINT8>(rowSize * sizeof(UINT8));

            UINT8* slicePtr = pixelData.GetData();
            for (UINT32 z = 0; z < depth; z++)
            {
                UINT32 halfHeight = height / 2;
                for (UINT32 y = 0; y < halfHeight; y++)
                {
                    UINT32 srcY = y * rowSize;
                    UINT32 dstY = (height - y - 1) * rowSize;

                    memcpy(rowTemp, &slicePtr[dstY], rowSize);
                    memcpy(&slicePtr[dstY], &slicePtr[srcY], rowSize);
                    memcpy(&slicePtr[srcY], rowTemp, rowSize);
                }

                // Note: If flipping X as well I could do it here without an extra set of memcpys

                slicePtr += pixelData.GetSlicePitch();
            }

            te_free(rowTemp);
        }

        if (mode & MirrorMode::X)
        {
            UINT8* elemTemp = te_allocate<UINT8>(elemSize * sizeof(UINT8));

            UINT8* slicePtr = pixelData.GetData();
            for (UINT32 z = 0; z < depth; z++)
            {
                UINT8* rowPtr = slicePtr;
                for (UINT32 y = 0; y < height; y++)
                {
                    UINT32 halfWidth = width / 2;
                    for (UINT32 x = 0; x < halfWidth; x++)
                    {
                        UINT32 srcX = x * elemSize;
                        UINT32 dstX = (width - x - 1) * elemSize;

                        memcpy(elemTemp, &rowPtr[dstX], elemSize);
                        memcpy(&rowPtr[dstX], &rowPtr[srcX], elemSize);
                        memcpy(&rowPtr[srcX], elemTemp, elemSize);
                    }

                    rowPtr += pixelData.GetRowPitch();
                }

                slicePtr += pixelData.GetSlicePitch();
            }

            te_free(elemTemp);
        }
    }

    void PixelUtil::Scale(const PixelData& src, PixelData& scaled, Filter filter)
    {
        assert(PixelUtil::IsAccessible(src.GetFormat()));
        assert(PixelUtil::IsAccessible(scaled.GetFormat()));

        PixelData temp;
        switch (filter)
        {
        default:
        case FILTER_NEAREST:
            if(src.GetFormat() == scaled.GetFormat())
            {
                // No intermediate buffer needed
                temp = scaled;
            }
            else
            {
                // Allocate temporary buffer of destination size in source format
                temp = PixelData(scaled.GetWidth(), scaled.GetHeight(), scaled.GetDepth(), src.GetFormat());
                temp.AllocateInternalBuffer();
            }

            // No conversion
            switch (PixelUtil::GetNumElemBytes(src.GetFormat()))
            {
            case 1: NearestResampler<1>::Scale(src, temp); break;
            case 2: NearestResampler<2>::Scale(src, temp); break;
            case 3: NearestResampler<3>::Scale(src, temp); break;
            case 4: NearestResampler<4>::Scale(src, temp); break;
            case 6: NearestResampler<6>::Scale(src, temp); break;
            case 8: NearestResampler<8>::Scale(src, temp); break;
            case 12: NearestResampler<12>::Scale(src, temp); break;
            case 16: NearestResampler<16>::Scale(src, temp); break;
            default:
                // Never reached
                assert(false);
            }

            if(temp.GetData() != scaled.GetData())
            {
                // Blit temp buffer
                PixelUtil::BulkPixelConversion(temp, scaled);

                temp.FreeInternalBuffer();
            }

            break;

        case FILTER_LINEAR:
            switch (src.GetFormat())
            {
            case PF_RG8:
            case PF_RGB8: case PF_BGR8:
            case PF_RGBA8: case PF_BGRA8:
                if(src.GetFormat() == scaled.GetFormat())
                {
                    // No intermediate buffer needed
                    temp = scaled;
                }
                else
                {
                    // Allocate temp buffer of destination size in source format
                    temp = PixelData(scaled.GetWidth(), scaled.GetHeight(), scaled.GetDepth(), src.GetFormat());
                    temp.AllocateInternalBuffer();
                }

                // No conversion
                switch (PixelUtil::GetNumElemBytes(src.GetFormat()))
                {
                case 1: LinearResampler_Byte<1>::Scale(src, temp); break;
                case 2: LinearResampler_Byte<2>::Scale(src, temp); break;
                case 3: LinearResampler_Byte<3>::Scale(src, temp); break;
                case 4: LinearResampler_Byte<4>::Scale(src, temp); break;
                default:
                    // Never reached
                    assert(false);
                }

                if(temp.GetData() != scaled.GetData())
                {
                    // Blit temp buffer
                    PixelUtil::BulkPixelConversion(temp, scaled);
                    temp.FreeInternalBuffer();
                }

                break;
            case PF_RGB32F:
            case PF_RGBA32F:
                if (scaled.GetFormat() == PF_RGB32F || scaled.GetFormat() == PF_RGBA32F)
                {
                    // float32 to float32, avoid unpack/repack overhead
                    LinearResampler_Float32::Scale(src, scaled);
                    break;
                }
                // Else, fall through
            default:
                // Fallback case, slow but works
                LinearResampler::Scale(src, scaled);
            }
            break;
        }
    }

    void PixelUtil::Copy(const PixelData& src, PixelData& dst, UINT32 offsetX, UINT32 offsetY, UINT32 offsetZ)
    {
        if(src.GetFormat() != dst.GetFormat())
        {
            TE_DEBUG("Source format is different from destination format for copy(). This operation cannot be used for a format conversion. Aborting copy.");
            return;
        }

        UINT32 right = offsetX + dst.GetWidth();
        UINT32 bottom = offsetY + dst.GetHeight();
        UINT32 back = offsetZ + dst.GetDepth();

        if(right > src.GetWidth() || bottom > src.GetHeight() || back > src.GetDepth())
        {
            TE_DEBUG("Provided offset or destination size is too large and is referencing pixels that are out of bounds on the source texture. Aborting copy().");
            return;
        }

        UINT8* srcPtr = (UINT8*)src.GetData() + offsetZ * src.GetSlicePitch();
        UINT8* dstPtr = (UINT8*)dst.GetData();

        UINT32 elemSize = GetNumElemBytes(dst.GetFormat());
        UINT32 rowSize = dst.GetWidth() * elemSize;

        for(UINT32 z = 0; z < dst.GetDepth(); z++)
        {
            UINT8* srcRowPtr = srcPtr + offsetY * src.GetRowPitch();
            UINT8* dstRowPtr = dstPtr;

            for(UINT32 y = 0; y < dst.GetHeight(); y++)
            {
                memcpy(dstRowPtr, srcRowPtr + offsetX * elemSize, rowSize);

                srcRowPtr += src.GetRowPitch();
                dstRowPtr += dst.GetRowPitch();
            }

            srcPtr += src.GetSlicePitch();
            dstPtr += dst.GetSlicePitch();
        }
    }

    PixelFormat PixelUtil::BestFormatFromFile(const String& path)
    {
        struct ExtensionToFormat
        {
            PixelFormat BigEndianFormat;
            PixelFormat LittleEndiantFormat;
        };

        static UnorderedMap<String, ExtensionToFormat> extensions =
        {
            {".jpeg", { PixelFormat::PF_RGB8, PixelFormat::PF_BGR8 } },
            {".jpg", { PixelFormat::PF_RGB8, PixelFormat::PF_BGR8 } },
            {".png", { PixelFormat::PF_RGBA8, PixelFormat::PF_BGRA8 } },
            {".dds", { PixelFormat::PF_RGBA8, PixelFormat::PF_BGRA8 } },
            {".tiff", { PixelFormat::PF_RGBA8, PixelFormat::PF_BGRA8 } },
            {".tif", { PixelFormat::PF_RGBA8, PixelFormat::PF_BGRA8 } },
            {".tga", { PixelFormat::PF_RGBA8, PixelFormat::PF_BGRA8 } },
            {".bmp", { PixelFormat::PF_RGB8, PixelFormat::PF_BGR8 } }
        };

        String extension = std::filesystem::path(path).extension().generic_string();
        auto it = extensions.find(extension);

        if (it != extensions.end())
            return Util::IsBigEndian() ? it->second.BigEndianFormat : it->second.LittleEndiantFormat;

        return Util::IsBigEndian() ? PF_RGBA8 : PF_BGRA8;
    }
}
