#pragma once

#include "TeCorePrerequisites.h"
#include "Importer/TeImportOptions.h"

namespace te
{
    /** Contains import options you may use to control how is a Mesh imported. */
    class TE_CORE_EXPORT MeshImportOptions : public ImportOptions
    {
    public:
        MeshImportOptions();

        /** Determines whether the texture data is also stored in CPU memory. */
        bool CpuCached = false;

        /** Determines should mesh normals be imported if available. */
        bool ImportNormals = true;

        /** Determines should mesh tangents and bitangents be imported if available. */
        bool ImportTangents = true;

        /** Determines should mesh skin data like bone weights, indices and bind poses be imported if available. */
        bool ImportSkin = false;

        /**	Determines should mesh blend shapes be imported if available. */
        bool ImportBlendShapes = false;

        /**	Determines should animation clips be imported if available. */
        bool ImportAnimation = false;

        /** Determine if we need to flip UV mapping when importing object */
        bool FplitUV = false;

        /** Determine if we need to change coordinate system */
        bool LeftHanded = false;

        /** Determine if we need to flip winding order in order to adjusts the output face winding order to be CW */
        bool FlipWinding = false;

        /** For FBX format, scale is in centimeters instead of meters, we must scale matrices using scale factor */
        bool ScaleSystemUnit = false;

        /** Factor used to convert unit system */
        float ScaleFactor = 0.01f;

        /** Determines if we need to set material properties for this mesh during import */
        bool ImportMaterials = true;

        /** Creates a new import options object that allows you to customize how are Meshs imported. */
        static SPtr<MeshImportOptions> Create();
    };
}
