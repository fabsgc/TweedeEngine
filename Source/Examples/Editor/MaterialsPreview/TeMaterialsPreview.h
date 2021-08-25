#pragma once

#include "TeCorePrerequisites.h"

#include "Renderer/TeRendererUtility.h"

namespace te
{
    class PreviewOpaqueMat;
    class PreviewTransparentMat;

    class MaterialsPreview
    {
    private:
        struct Preview
        {
            Preview();
            ~Preview();

            SPtr<RendererUtility::RenderTextureData> MatPreview;
            bool IsDirty = true;
        };

    public:
        MaterialsPreview();
        ~MaterialsPreview() = default;

        /**
         * Returns (and draw if necessary) the preview for the given material
         *
         * @param[in]	material		Weak Ref on a Material.
         */
        const RendererUtility::RenderTextureData& GetPreview(const WPtr<Material>& material);

        /**
         * Specifies that the given material preview need to be updated
         *
         * @param[in]	material		Weak Ref on a Material.
         */
        void MarkDirty(const WPtr<Material>& material);

        /**
         * Delete a material preview from the list
         *
         * @param[in]	material		Weak Ref on a Material.
         */
        void DeletePreview(const WPtr<Material>& material);

    private:
        /**
         * Draws a preview of the given material only if this material preview is not alreay managed by the class
         * or if this material preview is not up to date.
         *
         * @param[in]	material		Weak Ref on a Material.
         */
        void DrawMaterial(const WPtr<Material>& material, Preview& preview);

        /**
         * Initialize the camera used to draw previews to their render textures
         */
        void InitializeCamera();

        /**
         * Initialize the sphere used to display the material preview
         */
        void InitializeRenderable();

    private:
        Map<WPtr<Material>, UPtr<Preview>, std::owner_less<WPtr<Material>>> _previews;

        PreviewOpaqueMat* _opaqueMat;
        PreviewTransparentMat* _transparentMat;

        SPtr<Mesh> _mesh;
        SPtr<Camera> _camera;
    };
}
