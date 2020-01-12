#include "TePixelData.h"

namespace te
{
    UINT32 PixelData::GetSize() const
    {
        return 1;
    }

    UINT32 PixelData::GetInternalBufferSize() const
    {
        return GetSize();
    }
}
