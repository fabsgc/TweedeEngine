#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    /**	Kerning pair representing larger or smaller offset between a specific pair of characters. */
    struct TE_CORE_EXPORT KerningPair
    {
        UINT32 otherCharId;
        INT32 amount;
    };

    /**	Describes a single character in a font of a specific size. */
    struct TE_CORE_EXPORT CharDesc
    {
        UINT32 CharId; /**< Character ID, corresponding to a Unicode key. */
        UINT32 Page; /**< Index of the texture the character is located on. */
        float UvX, UvY; /**< Texture coordinates of the character in the page texture. */
        float UvWidth, UvHeight; /**< Width/height of the character in texture coordinates. */
        UINT32 Width, Height; /**< Width/height of the character in pixels. */
        INT32 XOffset, YOffset; /**< Offset for the visible portion of the character in pixels. */
        INT32 XAdvance, YAdvance; /**< Determines how much to advance the pen after writing this character, in pixels. */

        /**
         * Pairs that determine if certain character pairs should be closer or father together. for example "AV"
         * combination.
         */
        Vector<KerningPair> KerningPairs;
    };
}
