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
    RecursiveMutex PixelUtil::_recursiveMutex;
    Mutex PixelUtil::_mutex;

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
        PixelComponentLayout componentLayout; /**< Data layout of a single element of the format. */
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
        /* Component type, layout and count */
        PCT_BYTE, PCL_OTHER, 0,
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
        /* Component type, layout and count */
        PCT_BYTE, PCL_R8, 1,
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
        /* Component type, layout and count */
        PCT_BYTE, PCL_RG8, 2,
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
        /* Component type, layout and count */
        PCT_BYTE, PCL_RGB8, 3,
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
        /* Component type, layout and count */
        PCT_BYTE, PCL_BGR8, 3,
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
        /* Component type, layout and count */
        PCT_BYTE, PCL_BGRA8, 4,
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
        /* Component type, layout and count */
        PCT_BYTE, PCL_RGBA8, 4,
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
        /* Component type, layout and count */
        PCT_BYTE, PCL_OTHER, 3, // No alpha
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
        /* Component type, layout and count */
        PCT_BYTE, PCL_OTHER, 3,
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
        /* Component type, layout and count */
        PCT_BYTE, PCL_OTHER, 4,
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
        /* Component type, layout and count */
        PCT_BYTE, PCL_OTHER, 4,
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
        /* Component type, layout and count */
        PCT_BYTE, PCL_OTHER, 1,
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
        /* Component type, layout and count */
        PCT_BYTE, PCL_OTHER, 2,
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
        /* Component type, layout and count */
        PCT_FLOAT16, PCL_OTHER, 3,
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
        /* Component type, layout and count */
        PCT_BYTE, PCL_OTHER, 4,
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
        PFF_HALF,
        /* Component type, layout and count */
        PCT_FLOAT16, PCL_R16, 1,
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
        PFF_HALF,
        /* Component type, layout and count */
        PCT_FLOAT16, PCL_RG16, 2,
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
        PFF_HASALPHA | PFF_HALF,
        /* Component type, layout and count */
        PCT_FLOAT16, PCL_RGBA16, 4,
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
        /* Component type, layout and count */
        PCT_FLOAT32, PCL_R32, 1,
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
        /* Component type, layout and count */
        PCT_FLOAT32, PCL_RG32, 2,
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
        /* Component type, layout and count */
        PCT_FLOAT32, PCL_RGB32, 3,
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
        /* Component type, layout and count */
        PCT_FLOAT32, PCL_RGBA32, 4,
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
        PFF_DEPTH | PFF_NORMALIZED | PFF_DEPTH,
        /* Component type, layout and count */
        PCT_FLOAT32, PCL_R32, 2,
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
        PFF_DEPTH | PFF_INTEGER | PFF_NORMALIZED | PFF_DEPTH,
        /* Component type, layout and count */
        PCT_INT, PCL_OTHER, 2,
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
        PFF_DEPTH | PFF_FLOAT | PFF_DEPTH,
        /* Component type, layout and count */
        PCT_FLOAT32, PCL_R32, 1,
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
        PFF_DEPTH | PFF_INTEGER | PFF_NORMALIZED | PFF_DEPTH,
        /* Component type, layout and count */
        PCT_SHORT, PCL_R16, 1,
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
        /* Component type, layout and count */
        PCT_PACKED_R11G11B10, PCL_OTHER, 1,
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
        /* Component type, layout and count */
        PCT_PACKED_R10G10B10A2, PCL_OTHER, 1,
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
        /* Component type, layout and count */
        PCT_BYTE, PCL_R8, 1,
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
        /* Component type, layout and count */
        PCT_BYTE, PCL_RG8, 2,
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
        /* Component type, layout and count */
        PCT_BYTE, PCL_RGBA8, 4,
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
        /* Component type, layout and count */
        PCT_BYTE, PCL_R8, 1,
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
        /* Component type, layout and count */
        PCT_BYTE, PCL_RG8, 2,
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
        /* Component type, layout and count */
        PCT_BYTE, PCL_RGBA8, 4,
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
        /* Component type, layout and count */
        PCT_BYTE, PCL_R8, 1,
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
        /* Component type, layout and count */
        PCT_BYTE, PCL_RG8, 2,
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
        /* Component type, layout and count */
        PCT_BYTE, PCL_RGBA8, 4,
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
        /* Component type, layout and count */
        PCT_SHORT, PCL_R16, 1,
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
        /* Component type, layout and count */
        PCT_SHORT, PCL_RG16, 2,
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
        /* Component type, layout and count */
        PCT_SHORT, PCL_RGBA16, 4,
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
        /* Component type, layout and count */
        PCT_SHORT, PCL_R16, 1,
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
        /* Component type, layout and count */
        PCT_SHORT, PCL_RG16, 2,
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
        /* Component type, layout and count */
        PCT_SHORT, PCL_RGBA16, 4,
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
        /* Component type, layout and count */
        PCT_INT, PCL_R32, 1,
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
        /* Component type, layout and count */
        PCT_INT, PCL_RG32, 2,
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
        /* Component type, layout and count */
        PCT_INT, PCL_RGB32, 3,
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
        /* Component type, layout and count */
        PCT_INT, PCL_RGBA32, 4,
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
        /* Component type, layout and count */
        PCT_INT, PCL_R32, 1,
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
        /* Component type, layout and count */
        PCT_INT, PCL_RG32, 2,
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
        /* Component type, layout and count */
        PCT_INT, PCL_RGB32, 3,
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
        /* Component type, layout and count */
        PCT_INT, PCL_RGBA32, 4,
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
        /* Component type, layout and count */
        PCT_SHORT, PCL_R16, 1,
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
        /* Component type, layout and count */
        PCT_SHORT, PCL_RG16, 2,
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
        /* Component type, layout and count */
        PCT_SHORT, PCL_RGBA16, 4,
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
        /* Component type, layout and count */
        PCT_SHORT, PCL_R16, 1,
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
        /* Component type, layout and count */
        PCT_SHORT, PCL_RG16, 2,
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
        /* Component type, layout and count */
        PCT_SHORT, PCL_RGB16, 3,
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
        /* Component type, layout and count */
        PCT_SHORT, PCL_RGBA16, 4,
        /* rbits, gbits, bbits, abits */
        16, 16, 16, 16,
        /* Masks and shifts */
        0x0000FFFF, 0xFFFF0000, 0x0000FFFF, 0xFFFF0000,
        0, 16, 0, 16
        }
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

    PixelComponentLayout PixelUtil::GetElementLayout(PixelFormat format)
    {
        const PixelFormatDescription& des = GetDescriptionFor(format);
        return des.componentLayout;
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
        PackColor(&color.r, format, dest);
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
            float color[4] = {
                (float)r / 255.0f,
                (float)g / 255.0f,
                (float)b / 255.0f,
                (float)a / 255.0f
            };
            PackColor(&color[0], format, dest);
        }
    }
    
    void PixelUtil::PackColor(const float* rgbaPtr, PixelFormat format, void* dstPtr)
    {
        const UINT32 flags = GetFlags(format);
        switch (format)
        {
        case PF_RGBA32F:
            ConvertFromFloat<float>(rgbaPtr, dstPtr, 4u, flags);
            break;
        case PF_RGBA32U:
            ConvertFromFloat<UINT32>(rgbaPtr, dstPtr, 4u, flags);
            break;
        case PF_RGBA32I:
            ConvertFromFloat<INT32>(rgbaPtr, dstPtr, 4u, flags);
            break;
        case PF_RGB32F:
            ConvertFromFloat<float>(rgbaPtr, dstPtr, 3u, flags);
            break;
        case PF_RGB32U:
            ConvertFromFloat<UINT32>(rgbaPtr, dstPtr, 3u, flags);
            break;
        case PF_RGB32I:
            ConvertFromFloat<INT32>(rgbaPtr, dstPtr, 3u, flags);
            break;
        case PF_RGBA16F:
            ConvertFromFloat<UINT16>(rgbaPtr, dstPtr, 4u, flags);
            break;
        case PF_RGBA16:
            ConvertFromFloat<UINT16>(rgbaPtr, dstPtr, 4u, flags);
            break;
        case PF_RGBA16U:
            ConvertFromFloat<UINT16>(rgbaPtr, dstPtr, 4u, flags);
            break;
        case PF_RGBA16S:
            ConvertFromFloat<INT16>(rgbaPtr, dstPtr, 4u, flags);
            break;
        case PF_RGBA16I:
            ConvertFromFloat<INT16>(rgbaPtr, dstPtr, 4u, flags);
            break;
        case PF_RG32F:
            ConvertFromFloat<float>(rgbaPtr, dstPtr, 2u, flags);
            break;
        case PF_RG32U:
            ConvertFromFloat<UINT32>(rgbaPtr, dstPtr, 2u, flags);
            break;
        case PF_RG32I:
            ConvertFromFloat<INT32>(rgbaPtr, dstPtr, 2u, flags);
            break;
        case PF_D32_S8X24:
            ((float*)dstPtr)[0] = rgbaPtr[0];
            ((UINT32*)dstPtr)[1] = static_cast<UINT32>(rgbaPtr[1]) << 24u;
            break;
        case PF_RGB10A2:
        {
            const UINT32 ir = static_cast<UINT16>(Math::Saturate(rgbaPtr[0]) * 1023.0f + 0.5f);
            const UINT32 ig = static_cast<UINT16>(Math::Saturate(rgbaPtr[1]) * 1023.0f + 0.5f);
            const UINT32 ib = static_cast<UINT16>(Math::Saturate(rgbaPtr[2]) * 1023.0f + 0.5f);
            const UINT32 ia = static_cast<UINT16>(Math::Saturate(rgbaPtr[3]) * 3.0f + 0.5f);

            ((UINT32*)dstPtr)[0] = (ia << 30u) | (ib << 20u) | (ig << 10u) | (ir);
            break;
        }
        case PF_RGBA8:
            ConvertFromFloat<UINT8>(rgbaPtr, dstPtr, 4u, flags);
            break;
        case PF_RGBA8U:
            ConvertFromFloat<UINT8>(rgbaPtr, dstPtr, 4u, flags);
            break;
        case PF_RGBA8S:
            ConvertFromFloat<INT8>(rgbaPtr, dstPtr, 4u, flags);
            break;
        case PF_RGBA8I:
            ConvertFromFloat<INT8>(rgbaPtr, dstPtr, 4u, flags);
            break;
        case PF_RG16F:
            ConvertFromFloat<UINT16>(rgbaPtr, dstPtr, 2u, flags);
            break;
        case PF_RG16:
            ConvertFromFloat<UINT16>(rgbaPtr, dstPtr, 2u, flags);
            break;
        case PF_RG16U:
            ConvertFromFloat<UINT16>(rgbaPtr, dstPtr, 2u, flags);
            break;
        case PF_RG16S:
            ConvertFromFloat<INT16>(rgbaPtr, dstPtr, 2u, flags);
            break;
        case PF_RG16I:
            ConvertFromFloat<INT16>(rgbaPtr, dstPtr, 2u, flags);
            break;
        case PF_D32:
            ConvertFromFloat<float>(rgbaPtr, dstPtr, 1u, flags);
            break;
        case PF_R32F:
            ConvertFromFloat<float>(rgbaPtr, dstPtr, 1u, flags);
            break;
        case PF_R32U:
            ConvertFromFloat<UINT32>(rgbaPtr, dstPtr, 1u, flags);
            break;
        case PF_R32I:
            ConvertFromFloat<INT32>(rgbaPtr, dstPtr, 1u, flags);
            break;
        case PF_D24S8:
            ((UINT32*)dstPtr)[0] = (static_cast<UINT32>(rgbaPtr[1]) << 24u) |
                static_cast<UINT32>(roundf(rgbaPtr[0] * 16777215.0f));
            break;
        case PF_RG8:
            ConvertFromFloat<UINT8>(rgbaPtr, dstPtr, 2u, flags);
            break;
        case PF_RG8U:
            ConvertFromFloat<UINT8>(rgbaPtr, dstPtr, 2u, flags);
            break;
        case PF_RG8S:
            ConvertFromFloat<INT8>(rgbaPtr, dstPtr, 2u, flags);
            break;
        case PF_RG8I:
            ConvertFromFloat<INT8>(rgbaPtr, dstPtr, 2u, flags);
            break;
        case PF_R16F:
            ConvertFromFloat<UINT16>(rgbaPtr, dstPtr, 1u, flags);
            break;
        case PF_D16:
            ConvertFromFloat<UINT16>(rgbaPtr, dstPtr, 1u, flags);
            break;
        case PF_R16:
            ConvertFromFloat<UINT16>(rgbaPtr, dstPtr, 1u, flags);
            break;
        case PF_R16U:
            ConvertFromFloat<UINT16>(rgbaPtr, dstPtr, 1u, flags);
            break;
        case PF_R16S:
            ConvertFromFloat<INT16>(rgbaPtr, dstPtr, 1u, flags);
            break;
        case PF_R16I:
            ConvertFromFloat<INT16>(rgbaPtr, dstPtr, 1u, flags);
            break;
        case PF_R8:
            ConvertFromFloat<UINT8>(rgbaPtr, dstPtr, 1u, flags);
            break;
        case PF_R8U:
            ConvertFromFloat<UINT8>(rgbaPtr, dstPtr, 1u, flags);
            break;
        case PF_R8S:
            ConvertFromFloat<INT8>(rgbaPtr, dstPtr, 1u, flags);
            break;
        case PF_R8I:
            ConvertFromFloat<INT8>(rgbaPtr, dstPtr, 1u, flags);
            break;
        case PF_BGRA8:
            ((UINT8*)dstPtr)[0] = static_cast<UINT8>(Math::Saturate(rgbaPtr[2]) * 255.0f + 0.5f);
            ((UINT8*)dstPtr)[1] = static_cast<UINT8>(Math::Saturate(rgbaPtr[1]) * 255.0f + 0.5f);
            ((UINT8*)dstPtr)[2] = static_cast<UINT8>(Math::Saturate(rgbaPtr[0]) * 255.0f + 0.5f);
            ((UINT8*)dstPtr)[3] = static_cast<UINT8>(Math::Saturate(rgbaPtr[3]) * 255.0f + 0.5f);
            break;
        case PF_RGB8:
            ConvertFromFloat<UINT8>(rgbaPtr, dstPtr, 3u, flags);
            break;
        case PF_BGR8:
            ((UINT8*)dstPtr)[0] = static_cast<UINT8>(Math::Saturate(rgbaPtr[2]) * 255.0f + 0.5f);
            ((UINT8*)dstPtr)[1] = static_cast<UINT8>(Math::Saturate(rgbaPtr[1]) * 255.0f + 0.5f);
            ((UINT8*)dstPtr)[2] = static_cast<UINT8>(Math::Saturate(rgbaPtr[0]) * 255.0f + 0.5f);
            break;
        case PF_RGB16:
            ConvertFromFloat<UINT16>(rgbaPtr, dstPtr, 3u, flags);
            break;
        case PF_UNKNOWN:
            TE_DEBUG("Can't pack to PF_UNKNOW");
            break;
        }
    }

    void PixelUtil::UnpackColor(Color* color, PixelFormat format, const void* src)
    {
        UnpackColor(&color->r, format, src);
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
            float color[4];
            UnpackColor(&color[0], format, src);

            *r = (UINT8)Bitwise::UnormToUint(color[0], 8);
            *g = (UINT8)Bitwise::UnormToUint(color[1], 8);
            *b = (UINT8)Bitwise::UnormToUint(color[2], 8);
            *a = (UINT8)Bitwise::UnormToUint(color[3], 8);
        }
    }

    void PixelUtil::UnpackColor(float* rgbaPtr, PixelFormat format, const void* srcPtr)
    {
        // All other formats handled in a generic way
        const PixelFormatDescription& des = GetDescriptionFor(format);
        assert(des.componentCount <= 4);

        const UINT32 flags = GetFlags(format);
        switch (format)
        {
        case PF_RG11B10F:
        {
            UINT32 value = *((UINT32*)rgbaPtr);
            rgbaPtr[0] = Bitwise::Float11ToFloat(value);
            rgbaPtr[1] = Bitwise::Float11ToFloat(value >> 11);
            rgbaPtr[2] = Bitwise::Float10ToFloat(value >> 22);

            return;
            break;
        }
        case PF_RGBA32F:
            ConvertToFloat<float>(rgbaPtr, srcPtr, des.componentCount, flags);
            break;
        case PF_RGBA32U:
            ConvertToFloat<UINT32>(rgbaPtr, srcPtr, des.componentCount, flags);
            break;
        case PF_RGBA32I:
            ConvertToFloat<UINT32>(rgbaPtr, srcPtr, des.componentCount, flags);
            break;
        case PF_RGB32F:
            ConvertToFloat<float>(rgbaPtr, srcPtr, des.componentCount, flags);
            break;
        case PF_RGB32U:
            ConvertToFloat<UINT32>(rgbaPtr, srcPtr, des.componentCount, flags);
            break;
        case PF_RGB32I:
            ConvertToFloat<INT32>(rgbaPtr, srcPtr, des.componentCount, flags);
            break;
        case PF_RGBA16F:
            ConvertToFloat<UINT16>(rgbaPtr, srcPtr, des.componentCount, flags);
            break;
        case PF_RGBA16:
            ConvertToFloat<UINT16>(rgbaPtr, srcPtr, des.componentCount, flags);
            break;
        case PF_RGBA16U:
            ConvertToFloat<UINT16>(rgbaPtr, srcPtr, des.componentCount, flags);
            break;
        case PF_RGBA16S:
            ConvertToFloat<INT16>(rgbaPtr, srcPtr, des.componentCount, flags);
            break;
        case PF_RGBA16I:
            ConvertToFloat<INT16>(rgbaPtr, srcPtr, des.componentCount, flags);
            break;
        case PF_RG32F:
            ConvertToFloat<float>(rgbaPtr, srcPtr, des.componentCount, flags);
            break;
        case PF_RG32U:
            ConvertToFloat<UINT32>(rgbaPtr, srcPtr, des.componentCount, flags);
            break;
        case PF_RG32I:
            ConvertToFloat<INT32>(rgbaPtr, srcPtr, des.componentCount, flags);
            break;
        case PF_D32_S8X24:
            rgbaPtr[0] = ((const float*)srcPtr)[0];
            rgbaPtr[1] = static_cast<float>(((const UINT32*)srcPtr)[1] >> 24u);
            rgbaPtr[2] = 0.0f;
            rgbaPtr[3] = 1.0f;
            break;
        case PF_RGB10A2:
        {
            const UINT32 val = ((const UINT32*)srcPtr)[0];
            rgbaPtr[0] = static_cast<float>(val & 0x3FF) / 1023.0f;
            rgbaPtr[1] = static_cast<float>((val >> 10u) & 0x3FF) / 1023.0f;
            rgbaPtr[2] = static_cast<float>((val >> 20u) & 0x3FF) / 1023.0f;
            rgbaPtr[3] = static_cast<float>(val >> 30u) / 3.0f;
            break;
        }
        case PF_RGBA8:
            ConvertToFloat<UINT8>(rgbaPtr, srcPtr, 4u, flags);
            break;
        case PF_RGBA8U:
            ConvertToFloat<UINT8>(rgbaPtr, srcPtr, 4u, flags);
            break;
        case PF_RGBA8S:
            ConvertToFloat<INT8>(rgbaPtr, srcPtr, 4u, flags);
            break;
        case PF_RGBA8I:
            ConvertToFloat<INT8>(rgbaPtr, srcPtr, 4u, flags);
            break;
        case PF_RG16F:
            ConvertToFloat<UINT16>(rgbaPtr, srcPtr, 2u, flags);
            break;
        case PF_RG16:
            ConvertToFloat<UINT16>(rgbaPtr, srcPtr, 2u, flags);
            break;
        case PF_RG16U:
            ConvertToFloat<UINT16>(rgbaPtr, srcPtr, 2u, flags);
            break;
        case PF_RG16S:
            ConvertToFloat<INT16>(rgbaPtr, srcPtr, 2u, flags);
            break;
        case PF_RG16I:
            ConvertToFloat<INT16>(rgbaPtr, srcPtr, 2u, flags);
            break;
        case PF_D32:
            ConvertToFloat<float>(rgbaPtr, srcPtr, 1u, flags);
            break;
        case PF_R32F:
            ConvertToFloat<float>(rgbaPtr, srcPtr, 1u, flags);
            break;
        case PF_R32U:
            ConvertToFloat<UINT32>(rgbaPtr, srcPtr, 1u, flags);
            break;
        case PF_R32I:
            ConvertToFloat<INT32>(rgbaPtr, srcPtr, 1u, flags);
            break;
        case PF_D24S8:
            rgbaPtr[0] = static_cast<float>(((const UINT32*)srcPtr)[0] & 0x00FFFFFF) / 16777215.0f;
            rgbaPtr[1] = static_cast<float>(((const UINT32*)srcPtr)[0] >> 24u);
            rgbaPtr[2] = 0.0f;
            rgbaPtr[3] = 1.0f;
            break;
        case PF_RG8:
            ConvertToFloat<UINT8>(rgbaPtr, srcPtr, 2u, flags);
            break;
        case PF_RG8U:
            ConvertToFloat<UINT8>(rgbaPtr, srcPtr, 2u, flags);
            break;
        case PF_RG8S:
            ConvertToFloat<INT8>(rgbaPtr, srcPtr, 2u, flags);
            break;
        case PF_RG8I:
            ConvertToFloat<INT8>(rgbaPtr, srcPtr, 2u, flags);
            break;
        case PF_R16F:
            ConvertToFloat<UINT16>(rgbaPtr, srcPtr, 1u, flags);
            break;
        case PF_D16:
            ConvertToFloat<UINT16>(rgbaPtr, srcPtr, 1u, flags);
            break;
        case PF_R16:
            ConvertToFloat<UINT16>(rgbaPtr, srcPtr, 1u, flags);
            break;
        case PF_R16U:
            ConvertToFloat<UINT16>(rgbaPtr, srcPtr, 1u, flags);
            break;
        case PF_R16S:
            ConvertToFloat<INT16>(rgbaPtr, srcPtr, 1u, flags);
            break;
        case PF_R16I:
            ConvertToFloat<INT16>(rgbaPtr, srcPtr, 1u, flags);
            break;
        case PF_R8:
            ConvertToFloat<UINT8>(rgbaPtr, srcPtr, 1u, flags);
            break;
        case PF_R8U:
            ConvertToFloat<UINT8>(rgbaPtr, srcPtr, 1u, flags);
            break;
        case PF_R8S:
            ConvertToFloat<INT8>(rgbaPtr, srcPtr, 1u, flags);
            break;
        case PF_R8I:
            ConvertToFloat<INT8>(rgbaPtr, srcPtr, 1u, flags);
            break;
        case PF_BGRA8:
            ConvertToFloat<UINT8>(rgbaPtr, srcPtr, 4u, flags);
            std::swap(rgbaPtr[0], rgbaPtr[2]);
            break;
        case PF_RGB8:
            ConvertToFloat<UINT8>(rgbaPtr, srcPtr, 3u, flags);
            break;
        case PF_BGR8:
            ConvertToFloat<UINT8>(rgbaPtr, srcPtr, 3u, flags);
            std::swap(rgbaPtr[0], rgbaPtr[2]);
            break;
        case PF_RGB16:
            ConvertToFloat<UINT16>(rgbaPtr, srcPtr, 3u, flags);
            break;
        default:
            TE_DEBUG("UnpackColor() not implemented for format \"" + GetFormatName(format) + "\"");
            break;
        }
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

    template <typename T>
    void PixelUtil::ConvertFromFloat(const float* rgbaPtr, void* dstPtr, size_t numComponents,
        UINT32 flags)
    {
        for (size_t i = 0; i < numComponents; ++i)
        {
            if (flags & PFF_FLOAT)
                ((float*)dstPtr)[i] = rgbaPtr[i];
            else if (flags & PFF_HALF)
                ((UINT16*)dstPtr)[i] = Bitwise::FloatToHalf(rgbaPtr[i]);
            else if (flags & PFF_NORMALIZED)
            {
                float val = rgbaPtr[i];
                if (!(flags & PFF_SIGNED))
                {
                    val = Math::Saturate(val);
                    val *= (float)std::numeric_limits<T>::max();
                    ((T*)dstPtr)[i] = static_cast<T>(roundf(val));
                }
                else
                {
                    val = Math::Clamp(val, -1.0f, 1.0f);
                    val *= (float)std::numeric_limits<T>::max();
                    ((T*)dstPtr)[i] = static_cast<T>(roundf(val));
                }
            }
            else
                ((T*)dstPtr)[i] = static_cast<T>(roundf(rgbaPtr[i]));
        }
    }

    template <typename T>
    void PixelUtil::ConvertToFloat(float* rgbaPtr, const void* srcPtr, size_t numComponents,
        UINT32 flags)
    {
        for (size_t i = 0; i < numComponents; ++i)
        {
            if (flags & PFF_FLOAT)
                rgbaPtr[i] = ((const float*)srcPtr)[i];
            else if (flags & PFF_HALF)
                rgbaPtr[i] = Bitwise::HalfToFloat(((const UINT16*)srcPtr)[i]);
            else if (flags & PFF_NORMALIZED)
            {
                const float val = static_cast<float>(((const T*)srcPtr)[i]);
                float rawValue = val / (float)std::numeric_limits<T>::max();
                if (!(flags & PFF_SIGNED))
                {
                    rgbaPtr[i] = rawValue;
                }
                else
                {
                    // -128 & -127 and -32768 & -32767 both map to -1 according to D3D10 rules.
                    rgbaPtr[i] = std::max(rawValue, -1.0f);
                }
            }
            else
            {
                rgbaPtr[i] = static_cast<float>(((const T*)srcPtr)[i]);
            }
        }

        // Set remaining components to 0, and alpha to 1
        for (size_t i = numComponents; i < 3u; ++i)
            rgbaPtr[i] = 0.0f;
        if (numComponents < 4u)
            rgbaPtr[3] = 1.0f;
    }

    typedef void (*row_conversion_func_t)(UINT8* src, UINT8* dst, size_t width);

    void ConvCopy16Bpx(UINT8* src, UINT8* dst, size_t width) { memcpy(dst, src, 16 * width); }
    void ConvCopy12Bpx(UINT8* src, UINT8* dst, size_t width) { memcpy(dst, src, 12 * width); }
    void ConvCopy8Bpx(UINT8* src, UINT8* dst, size_t width) { memcpy(dst, src, 8 * width); }
    void ConvCopy6Bpx(UINT8* src, UINT8* dst, size_t width) { memcpy(dst, src, 6 * width); }
    void ConvCopy4Bpx(UINT8* src, UINT8* dst, size_t width) { memcpy(dst, src, 4 * width); }
    void ConvCopy3Bpx(UINT8* src, UINT8* dst, size_t width) { memcpy(dst, src, 3 * width); }
    void ConvCopy2Bpx(UINT8* src, UINT8* dst, size_t width) { memcpy(dst, src, 2 * width); }
    void ConvCopy1Bpx(UINT8* src, UINT8* dst, size_t width) { memcpy(dst, src, 1 * width); }

    // clang-format off
    void ConvRGBA32toRGB32(UINT8* _src, UINT8* _dst, size_t width)
    {
        UINT32* src = (UINT32*)_src; UINT32* dst = (UINT32*)_dst;
        while (width--) { dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2]; src += 4; dst += 3; }
    }
    void ConvRGB32toRG32(UINT8* _src, UINT8* _dst, size_t width)
    {
        UINT32* src = (UINT32*)_src; UINT32* dst = (UINT32*)_dst;
        while (width--) { dst[0] = src[0]; dst[1] = src[1]; src += 3; dst += 2; }
    }
    void ConvRG32toRGB32(UINT8* _src, UINT8* _dst, size_t width)
    {
        UINT32* src = (UINT32*)_src; UINT32* dst = (UINT32*)_dst;
        while (width--) { dst[0] = src[0]; dst[1] = src[1]; dst[2] = 0u; src += 2; dst += 3; }
    }
    void ConvRG32toR32(UINT8* _src, UINT8* _dst, size_t width)
    {
        UINT32* src = (UINT32*)_src; UINT32* dst = (UINT32*)_dst;
        while (width--) { dst[0] = src[0]; src += 2; dst += 1; }
    }

    void ConvRGBA16toRGB16(UINT8* _src, UINT8* _dst, size_t width)
    {
        UINT16* src = (UINT16*)_src; UINT16* dst = (UINT16*)_dst;
        while (width--) { dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2]; src += 4; dst += 3; }
    }
    void ConvRGB16toRGBA16(UINT8* _src, UINT8* _dst, size_t width)
    {
        UINT16* src = (UINT16*)_src; UINT16* dst = (UINT16*)_dst;
        while (width--) { dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2]; dst[3] = 0xFFFF; src += 3; dst += 4; }
    }
    void ConvRGB16toRG16(UINT8* _src, UINT8* _dst, size_t width)
    {
        UINT16* src = (UINT16*)_src; UINT16* dst = (UINT16*)_dst;
        while (width--) { dst[0] = src[0]; dst[1] = src[1]; src += 3; dst += 2; }
    }
    void ConvRG16toRGB16(UINT8* _src, UINT8* _dst, size_t width)
    {
        UINT16* src = (UINT16*)_src; UINT16* dst = (UINT16*)_dst;
        while (width--) { dst[0] = src[0]; dst[1] = src[1]; dst[0] = 0u; src += 2; dst += 3; }
    }
    void ConvRG16toR16(UINT8* _src, UINT8* _dst, size_t width)
    {
        UINT16* src = (UINT16*)_src; UINT16* dst = (UINT16*)_dst;
        while (width--) { dst[0] = src[0]; src += 2; dst += 1; }
    }

    void ConvRGBAtoBGRA(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = src[2]; dst[1] = src[1]; dst[2] = src[0]; dst[3] = src[3]; src += 4; dst += 4; }
    }
    void ConvRGBAtoRGB(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2]; src += 4; dst += 3; }
    }
    void ConvRGBAtoBGR(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = src[2]; dst[1] = src[1]; dst[2] = src[0]; src += 4; dst += 3; }
    }
    void ConvRGBAtoRG(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = src[0]; dst[1] = src[1]; src += 4; dst += 2; }
    }
    void ConvRGBAtoRG_u2s(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = src[0] - 128; dst[1] = src[1] - 128; src += 4; dst += 2; }
    }
    void ConvRGBAtoRG_s2u(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = src[0] + 128; dst[1] = src[1] + 128; src += 4; dst += 2; }
    }
    void ConvRGBAtoR(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = src[0]; src += 4; dst += 1; }
    }

    void ConvBGRAtoRG(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = src[2]; dst[1] = src[1]; src += 4; dst += 2; }
    }
    void ConvBGRAtoRG_u2s(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = src[2] - 128; dst[1] = src[1] - 128; src += 4; dst += 2; }
    }
    void ConvBGRAtoRG_s2u(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = src[2] + 128; dst[1] = src[1] + 128; src += 4; dst += 2; }
    }
    void ConvBGRAtoR(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = src[2]; src += 4; dst += 1; }
    }

    void ConvBGRXtoRGBA(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = src[2]; dst[1] = src[1]; dst[2] = src[0]; dst[3] = 0xFF; src += 4; dst += 4; }
    }
    void ConvBGRXtoBGRA(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2]; dst[3] = 0xFF; src += 4; dst += 4; }
    }

    void ConvRGBtoRGBA(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2]; dst[3] = 0xFF; src += 3; dst += 4; }
    }
    void ConvRGBtoBGRA(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = src[2]; dst[1] = src[1]; dst[2] = src[0]; dst[3] = 0xFF; src += 3; dst += 4; }
    }
    void ConvRGBtoBGR(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = src[2]; dst[1] = src[1]; dst[2] = src[0]; src += 3; dst += 3; }
    }
    void ConvRGBtoRG(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = src[0]; dst[1] = src[1]; src += 3; dst += 2; }
    }
    void ConvRGBtoRG_u2s(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = src[0] - 128; dst[1] = src[1] - 128; src += 3; dst += 2; }
    }
    void ConvRGBtoRG_s2u(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = src[0] + 128; dst[1] = src[1] + 128; src += 3; dst += 2; }
    }
    void ConvRGBtoR(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = src[0]; src += 3; dst += 1; }
    }

    void ConvBGRtoRG(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = src[2]; dst[1] = src[1]; src += 3; dst += 2; }
    }
    void ConvBGRtoRG_u2s(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = src[2] - 128; dst[1] = src[1] - 128; src += 3; dst += 2; }
    }
    void ConvBGRtoRG_s2u(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = src[2] + 128; dst[1] = src[1] + 128; src += 3; dst += 2; }
    }
    void ConvBGRtoR(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = src[2]; src += 3; dst += 1; }
    }

    void ConvRGtoRGB(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = src[0]; dst[1] = src[1]; dst[2] = 0u; src += 2; dst += 3; }
    }
    void ConvRGtoBGR(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = 0u; dst[1] = src[1]; dst[2] = src[0]; src += 2; dst += 3; }
    }
    void ConvRGtoRG_u2s(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = src[0] - 128; dst[1] = src[1] - 128; src += 2; dst += 2; }
    }
    void ConvRGtoRG_s2u(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = src[0] + 128; dst[1] = src[1] + 128; src += 2; dst += 2; }
    }
    void ConvRGtoR(UINT8* src, UINT8* dst, size_t width)
    {
        while (width--) { dst[0] = src[0]; src += 2; dst += 1; }
    }

    void PixelUtil::BulkPixelConversion(const PixelData &src, PixelData &dst)
    {
        if(src.GetWidth() != dst.GetWidth() || src.GetHeight() != dst.GetHeight() || src.GetDepth() != dst.GetDepth())
        {
            TE_DEBUG("Cannot convert pixels between buffers of different sizes.");
            return;
        }

        // Is there a optimized row conversion?
        row_conversion_func_t rowConversionFunc = 0;
        assert(PCL_COUNT <= 16);  // adjust PCL_PAIR definition if assertion failed
#define PCL_PAIR( a, b ) ( ( a << 4 ) | b )

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
        else if (GetFlags(src.GetFormat()) == GetFlags(dst.GetFormat())) // semantic match, copy as typeless
        {
            PixelComponentLayout srcLayout = GetElementLayout(src.GetFormat());
            PixelComponentLayout dstLayout = GetElementLayout(dst.GetFormat());
            switch (PCL_PAIR(srcLayout, dstLayout))
            {
                case PCL_PAIR(PCL_RGBA32, PCL_RGB32): rowConversionFunc = ConvRGBA32toRGB32; break;
                case PCL_PAIR(PCL_RGB32, PCL_RG32): rowConversionFunc = ConvRGB32toRG32; break;
                case PCL_PAIR(PCL_RG32, PCL_RGB32): rowConversionFunc = ConvRG32toRGB32; break;
                case PCL_PAIR(PCL_RG32, PCL_R32): rowConversionFunc = ConvRG32toR32; break;

                case PCL_PAIR(PCL_RGBA16, PCL_RGB16): rowConversionFunc = ConvRGBA16toRGB16; break;
                case PCL_PAIR(PCL_RGB16, PCL_RGBA16): rowConversionFunc = ConvRGB16toRGBA16; break;
                case PCL_PAIR(PCL_RGB16, PCL_RG16): rowConversionFunc = ConvRGB16toRG16; break;
                case PCL_PAIR(PCL_RG16, PCL_RGB16): rowConversionFunc = ConvRG16toRGB16; break;
                case PCL_PAIR(PCL_RG16, PCL_R16): rowConversionFunc = ConvRG16toR16; break;

                case PCL_PAIR(PCL_RGBA8, PCL_BGRA8): rowConversionFunc = ConvRGBAtoBGRA; break;
                case PCL_PAIR(PCL_RGBA8, PCL_BGRX8): rowConversionFunc = ConvRGBAtoBGRA; break;
                case PCL_PAIR(PCL_RGBA8, PCL_RGB8): rowConversionFunc = ConvRGBAtoRGB; break;
                case PCL_PAIR(PCL_RGBA8, PCL_BGR8): rowConversionFunc = ConvRGBAtoBGR; break;
                case PCL_PAIR(PCL_RGBA8, PCL_RG8): rowConversionFunc = ConvRGBAtoRG; break;
                case PCL_PAIR(PCL_RGBA8, PCL_R8): rowConversionFunc = ConvRGBAtoR; break;

                case PCL_PAIR(PCL_BGRA8, PCL_RGBA8): rowConversionFunc = ConvRGBAtoBGRA; break;
                case PCL_PAIR(PCL_BGRA8, PCL_BGRX8): rowConversionFunc = ConvCopy4Bpx; break;
                case PCL_PAIR(PCL_BGRA8, PCL_RGB8): rowConversionFunc = ConvRGBAtoBGR; break;
                case PCL_PAIR(PCL_BGRA8, PCL_BGR8): rowConversionFunc = ConvRGBAtoRGB; break;
                case PCL_PAIR(PCL_BGRA8, PCL_RG8): rowConversionFunc = ConvBGRAtoRG; break;
                case PCL_PAIR(PCL_BGRA8, PCL_R8): rowConversionFunc = ConvBGRAtoR; break;

                case PCL_PAIR(PCL_BGRX8, PCL_RGBA8): rowConversionFunc = ConvBGRXtoRGBA; break;
                case PCL_PAIR(PCL_BGRX8, PCL_BGRA8): rowConversionFunc = ConvBGRXtoBGRA; break;
                case PCL_PAIR(PCL_BGRX8, PCL_RGB8): rowConversionFunc = ConvRGBAtoBGR; break;
                case PCL_PAIR(PCL_BGRX8, PCL_BGR8): rowConversionFunc = ConvRGBAtoRGB; break;
                case PCL_PAIR(PCL_BGRX8, PCL_RG8): rowConversionFunc = ConvBGRAtoRG; break;
                case PCL_PAIR(PCL_BGRX8, PCL_R8): rowConversionFunc = ConvBGRAtoR; break;

                case PCL_PAIR(PCL_RGB8, PCL_RGBA8): rowConversionFunc = ConvRGBtoRGBA; break;
                case PCL_PAIR(PCL_RGB8, PCL_BGRA8): rowConversionFunc = ConvRGBtoBGRA; break;
                case PCL_PAIR(PCL_RGB8, PCL_BGRX8): rowConversionFunc = ConvRGBtoBGRA; break;
                case PCL_PAIR(PCL_RGB8, PCL_BGR8): rowConversionFunc = ConvRGBtoBGR; break;
                case PCL_PAIR(PCL_RGB8, PCL_RG8): rowConversionFunc = ConvRGBtoRG; break;
                case PCL_PAIR(PCL_RGB8, PCL_R8): rowConversionFunc = ConvRGBtoR; break;

                case PCL_PAIR(PCL_BGR8, PCL_RGBA8): rowConversionFunc = ConvRGBtoBGRA; break;
                case PCL_PAIR(PCL_BGR8, PCL_BGRA8): rowConversionFunc = ConvRGBtoRGBA; break;
                case PCL_PAIR(PCL_BGR8, PCL_BGRX8): rowConversionFunc = ConvRGBtoRGBA; break;
                case PCL_PAIR(PCL_BGR8, PCL_RGB8): rowConversionFunc = ConvRGBAtoBGR; break;
                case PCL_PAIR(PCL_BGR8, PCL_RG8): rowConversionFunc = ConvBGRtoRG; break;
                case PCL_PAIR(PCL_BGR8, PCL_R8): rowConversionFunc = ConvBGRtoR; break;

                case PCL_PAIR(PCL_RG8, PCL_RGB8): rowConversionFunc = ConvRGtoRGB; break;
                case PCL_PAIR(PCL_RG8, PCL_BGR8): rowConversionFunc = ConvRGtoBGR; break;
                case PCL_PAIR(PCL_RG8, PCL_R8): rowConversionFunc = ConvRGtoR; break;
            }
        }
        else if (GetFlags(src.GetFormat()) == PFF_NORMALIZED && GetFlags(dst.GetFormat()) == (PFF_NORMALIZED | PFF_SIGNED))
        {
            PixelComponentLayout srcLayout = GetElementLayout(src.GetFormat());
            PixelComponentLayout dstLayout = GetElementLayout(dst.GetFormat());
            switch (PCL_PAIR(srcLayout, dstLayout))
            {
                case PCL_PAIR(PCL_RGBA8, PCL_RG8): rowConversionFunc = ConvRGBAtoRG_u2s; break;
                case PCL_PAIR(PCL_BGRA8, PCL_RG8): rowConversionFunc = ConvBGRAtoRG_u2s; break;
                case PCL_PAIR(PCL_BGRX8, PCL_RG8): rowConversionFunc = ConvBGRAtoRG_u2s; break;
                case PCL_PAIR(PCL_RGB8, PCL_RG8): rowConversionFunc = ConvRGBtoRG_u2s; break;
                case PCL_PAIR(PCL_BGR8, PCL_RG8): rowConversionFunc = ConvBGRtoRG_u2s; break;
                case PCL_PAIR(PCL_RG8, PCL_RG8): rowConversionFunc = ConvRGtoRG_u2s; break;
            }
        }
        else if (GetFlags(src.GetFormat()) == (PFF_NORMALIZED | PFF_SIGNED) && GetFlags(dst.GetFormat()) == PFF_NORMALIZED)
        {
            PixelComponentLayout srcLayout = GetElementLayout(src.GetFormat());
            PixelComponentLayout dstLayout = GetElementLayout(dst.GetFormat());
            switch (PCL_PAIR(srcLayout, dstLayout))
            {
                case PCL_PAIR(PCL_RGBA8, PCL_RG8): rowConversionFunc = ConvRGBAtoRG_s2u; break;
                case PCL_PAIR(PCL_BGRA8, PCL_RG8): rowConversionFunc = ConvBGRAtoRG_s2u; break;
                case PCL_PAIR(PCL_BGRX8, PCL_RG8): rowConversionFunc = ConvBGRAtoRG_s2u; break;
                case PCL_PAIR(PCL_RGB8, PCL_RG8): rowConversionFunc = ConvRGBtoRG_s2u; break;
                case PCL_PAIR(PCL_BGR8, PCL_RG8): rowConversionFunc = ConvBGRtoRG_s2u; break;
                case PCL_PAIR(PCL_RG8, PCL_RG8): rowConversionFunc = ConvRGtoRG_s2u; break;
            }
        }
#undef PCL_PAIR

        if (rowConversionFunc)
        {
            UINT32 srcPixelSize = GetNumElemBytes(src.GetFormat());
            UINT32 dstPixelSize = GetNumElemBytes(dst.GetFormat());

            UINT8* srcData = static_cast<UINT8*>(src.GetData())
                + src.GetLeft() * srcPixelSize + src.GetTop() * src.GetRowPitch() + src.GetFront() * src.GetSlicePitch();
            UINT8* dstData = static_cast<UINT8*>(dst.GetData())
                + dst.GetLeft() * dstPixelSize + dst.GetTop() * dst.GetRowPitch() + dst.GetFront() * dst.GetSlicePitch();

            // Get pitches+skips in bytes
            //const UINT32 srcRowPitchBytes = src.GetRowPitch();
            //const UINT32 srcSliceSkipBytes = src.GetSliceSkip();

            //const UINT32 dstRowPitchBytes = dst.GetRowPitch();
            //const UINT32 dstSliceSkipBytes = dst.GetSliceSkip();

            for (UINT32 z = src.GetFront(); z < src.GetBack(); z++)
            {
                for (UINT32 y = src.GetTop(); y < src.GetBottom(); y++)
                {
                    UINT8* srcPtr = srcData + src.GetSlicePitch() * z + src.GetRowPitch() * y;
                    UINT8* dstPtr = dstData + dst.GetSlicePitch() * z + dst.GetRowPitch() * y;
                    rowConversionFunc(srcPtr, dstPtr, src.GetWidth());
                }
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
        float color[4];

        for (UINT32 z = src.GetFront(); z < src.GetBack(); z++)
        {
            for (UINT32 y = src.GetTop(); y < src.GetBottom(); y++)
            {
                for (UINT32 x = src.GetLeft(); x < src.GetRight(); x++)
                {
                    UnpackColor(color, src.GetFormat(), srcptr);
                    PackColor(color, dst.GetFormat(), dstptr);

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

        PixelFormat interimFormat;

        interimFormat = IsFloatingPoint(src.GetFormat()) ? PF_RGBA32F : PF_BGRA8;

        PixelData interimData(src.GetWidth(), src.GetHeight(), 1, interimFormat);
        interimData.AllocateInternalBuffer();
        BulkPixelConversion(src, interimData);

        if (interimFormat != PF_RGBA32F)
            FlipComponentOrder(interimData);

        UINT32 numMips = GetMaxMipmaps(src.GetWidth(), src.GetHeight(), 1);
        if (maxMip > 0)
            numMips = std::min(numMips, maxMip);

        Lock lock(_mutex);

        nvtt::InputOptions io;
        io.setTextureLayout(nvtt::TextureType_2D, src.GetWidth(), src.GetHeight());
        io.setMipmapGeneration(true, numMips);
        io.setNormalMap(options.IsNormalMap);
        io.setNormalizeMipmaps(options.NormalizeMipmaps);
        io.setWrapMode(ToNVTTWrapMode(options.WrapMode));
        io.setAlphaMode(ToNVTTAlphaMode(options.Alpha));
        io.setMipmapFilter(ToNVTTFilter(options.Filter));
        io.setMipmapData(interimData.GetData(), src.GetWidth(), src.GetHeight());

        interimData.FreeInternalBuffer();

        switch (interimFormat)
        {
            case PF_RGBA32F:
                io.setFormat(nvtt::InputFormat_RGBA_32F);
                break;
            case PF_BGRA8:
                io.setFormat(nvtt::InputFormat_BGRA_8UB);
                break;
            case PF_RGB32F:
                io.setFormat(nvtt::InputFormat_RGBA_32F);
                break;
            case PF_BGR8:
                io.setFormat(nvtt::InputFormat_BGRA_8UB);
                break;
        }

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

        RecursiveLock lock(_recursiveMutex);

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
