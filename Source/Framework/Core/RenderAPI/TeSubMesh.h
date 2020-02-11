#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    /** Data about a sub-mesh range and the type of primitives contained in the range. */
    struct TE_CORE_EXPORT SubMesh
    {
        SubMesh() = default;

        SubMesh(UINT32 indexOffset, UINT32 indexCount, DrawOperationType drawOp, const String& materialName = "")
            : IndexOffset(indexOffset)
            , IndexCount(indexCount)
            , DrawOp(drawOp)
            , MaterialName(materialName)
        { }

        UINT32 IndexOffset = 0;
        UINT32 IndexCount = 0;
        DrawOperationType DrawOp = DOT_TRIANGLE_LIST;

        /** It's possible to set a material name which will be use if you want to SetMaterial() on a mesh */
        String MaterialName = "";
    };
}
