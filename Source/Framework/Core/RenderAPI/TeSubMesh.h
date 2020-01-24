#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    /** Data about a sub-mesh range and the type of primitives contained in the range. */
    struct TE_CORE_EXPORT SubMesh
    {
        SubMesh() = default;

        SubMesh(UINT32 indexOffset, UINT32 indexCount, DrawOperationType drawOp)
            : IndexOffset(indexOffset)
            , IndexCount(indexCount)
            , DrawOp(drawOp)
        { }

        UINT32 IndexOffset = 0;
        UINT32 IndexCount = 0;
        DrawOperationType DrawOp = DOT_TRIANGLE_LIST;
    };
}
