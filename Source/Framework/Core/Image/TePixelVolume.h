#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    /** Represents a 3D region of pixels used for referencing pixel data. */
    struct TE_CORE_EXPORT PixelVolume
    {
        UINT32 Left = 0;
        UINT32 Top = 0;
        UINT32 Right = 1;
        UINT32 Bottom = 1;
        UINT32 Front = 0;
        UINT32 Back = 1;

        PixelVolume() = default;

        PixelVolume(UINT32 left, UINT32 top, UINT32 right, UINT32 bottom):
            Left(left), Top(top), Right(right), Bottom(bottom), Front(0), Back(1)
        {
            assert(Right >= Left && Bottom >= Top && Back >= Front);
        }

        PixelVolume(UINT32 left, UINT32 top, UINT32 front, UINT32 right, UINT32 bottom, UINT32 back):
            Left(left), Top(top), Right(right), Bottom(bottom), Front(front), Back(back)
        {
            assert(Right >= Left && Bottom >= Top && Back >= Front);
        }
            
        /** Return true if the other box is a part of this one. */
        bool Contains(const PixelVolume &volume) const
        {
            return (volume.Left >= Left && volume.Top >= Top && volume.Front >= Front &&
                volume.Right <= Right && volume.Bottom <= Bottom && volume.Back <= Back);
        }
            
        UINT32 GetWidth() const { return Right-Left; }
        UINT32 GetHeight() const { return Bottom-Top; }
        UINT32 GetDepth() const { return Back-Front; }
    };

    /** @} */
}
