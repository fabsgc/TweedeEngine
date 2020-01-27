#pragma once

#include "TeCorePrerequisites.h"
#include "Importer/TeImportOptions.h"

namespace te
{
    /** Contains import options you may use to control how is a Mesh imported. */
    class TE_CORE_EXPORT MeshImportOptions : public ImportOptions
    {
    public:
        MeshImportOptions() = default;

        /** Determines whether the texture data is also stored in CPU memory. */
        bool CpuCached = false;

        /** Determines should mesh normals be imported if available. */
        bool ImportNormals = true;

        /** Determines should mesh tangents and bitangents be imported if available. */
        bool ImportTangents = true;

        /** Uniformly scales the imported mesh by the specified value. */
        float ImportScale = 1.0f;

        /** Determines should mesh skin data like bone weights, indices and bind poses be imported if available. */
        bool ImportSkin = false;

        /** Determine if we need to flip UV mapping when importing object */
        bool SplitUV = true;

        /** Determine if we need to change coordinate system */
        bool LeftHanded = false;

        /** Determine if we need to flip winding order in order to adjusts the output face winding order to be CW */
        bool FlipWinding = false;

        /** Creates a new import options object that allows you to customize how are Meshs imported. */
        static SPtr<MeshImportOptions> Create();
    };
}
