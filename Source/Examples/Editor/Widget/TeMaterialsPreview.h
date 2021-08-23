#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    class MaterialsPreview
    {
    public:
        MaterialsPreview();
        ~MaterialsPreview() = default;

        /**
         * Draw a preview of the given material only if this material preview is not alreay managed by the class
         * or if this material preview is not up to date.
         *
         * @param[in]	material		Weak Ref on a Material.
         */
        void DrawMaterial(WPtr<Material> material);

        /**
         * Specify that the given material preview need to be updated
         *
         * @param[in]	material		Weak Ref on a Material.
         */
        void MarkDirty(WPtr<Material> material);
    };
}
