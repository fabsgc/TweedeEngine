#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    /** Data about a sub-mesh range and the type of primitives contained in the range. */
    struct TE_CORE_EXPORT SubMesh
    {
        SubMesh() = default;

        SubMesh(UINT32 indexOffset, UINT32 indexCount, DrawOperationType drawOp, const String& materialName = "", const String& name = "")
            : IndexOffset(indexOffset)
            , IndexCount(indexCount)
            , DrawOp(drawOp)
            , MaterialName(materialName)
            , Name(name)
        { }

        UINT32 IndexOffset = 0;
        UINT32 IndexCount = 0;
        DrawOperationType DrawOp = DOT_LINE_STRIP;

        /** It's possible to set a material name which will be use if you want to SetMaterial() on a mesh */
        String MaterialName = "";
        /** It's easier to have a name for identification in editor */
        String Name = "";
    };
}
