#include "Renderer/TeIBLUtility.h"
#include "Math/TeVector2I.h"

namespace te
{
    const UINT32 IBLUtility::REFLECTION_CUBEMAP_SIZE = 512;
    const UINT32 IBLUtility::IRRADIANCE_CUBEMAP_SIZE = 32;

    TE_MODULE_STATIC_MEMBER(IBLUtility)

    /** Returns the size of the texture required to store the provided number of SH coefficients. */
    Vector2I IBLUtility::GetSHCoeffTextureSize(UINT32 numCoeffSets, UINT32 shOrder)
    {
        UINT32 coeffsPerSet = shOrder * shOrder;

        // Assuming the texture maximum size is 4096
        UINT32 maxSetsPerRow = 4096 / coeffsPerSet;

        Vector2I output;
        output.x = (numCoeffSets > maxSetsPerRow ? maxSetsPerRow : numCoeffSets) * coeffsPerSet;
        output.y = 1 + numCoeffSets / (maxSetsPerRow + 1);

        return output;
    }

    /** Determines the position of a set of coefficients in the coefficient texture, depending on the coefficient index. */
    Vector2I IBLUtility::GetSHCoeffXYFromIdx(UINT32 idx, UINT32 shOrder)
    {
        UINT32 coeffsPerSet = shOrder * shOrder;

        // Assuming the texture maximum size is 4096
        UINT32 maxSetsPerRow = 4096 / coeffsPerSet;

        Vector2I output;
        output.x = (idx % maxSetsPerRow) * coeffsPerSet;
        output.y = idx / maxSetsPerRow;

        return output;
    }
}
