#pragma once
#include "Prerequisites/TePrerequisitesUtility.h"
#include <windows.h>

namespace te
{
    /** Provides access to various Windows specific utility functions. */
    class TE_UTILITY_EXPORT Win32PlatformUtility
    {
    public:
        /**
         * Creates a new bitmap usable by various Win32 methods from the provided pixel data. Caller must ensure to call
         * DeleteObject() on the bitmap handle when finished.
         */
        static HBITMAP CreateBitmap(const Color* pixels, UINT32 width, UINT32 height, bool premultiplyAlpha);
    };
}
