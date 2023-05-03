#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    class EditorUtils
    {
    public:
        static const String DELETE_BINDING;
        static const String COPY_BINDING;
        static const String PASTE_BINDING;

    public:
        /** When we load a mesh using with GUI, we can generate materials to apply on its submeshes */
        static void ImportMeshMaterials(HMesh& mesh, bool SRGB);

        /** */
        static void ApplyDefaultMaterial(HMesh& mesh);
    };
}
