#pragma once

#include "TeCorePrerequisites.h"
#include "Material/TeMaterial.h"

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

        ~SubMesh()
        { }

        UINT32 IndexOffset = 0;
        UINT32 IndexCount = 0;
        DrawOperationType DrawOp = DOT_TRIANGLE_LIST;

        /** It's possible to set a material name which will be use if you want to SetMaterial() on a mesh */
        String MaterialName = "";
        /** It's easier to have a name for identification in editor */
        String Name = "";

        /** During mesh import, it's possible to get information about the default material applied on this submesh */
        MaterialProperties MatProperties;

        /** During mesh import, it's possible to get information about the default material applied on this submesh */
        MaterialTextures MatTextures;

        /** We can store here the "default" material applied to this submesh, useful when loading a mesh */
        HMaterial Mat;
    };
}
