#pragma once

#include "TeRenderManPrerequisites.h"
#include "Renderer/TeRenderElement.h"
#include "Renderer/TeRendererMaterial.h"

namespace te
{
    class Decal;

    /** Default material used for rendering decals, when no other is available. */
    class DefaultDecalMat : public RendererMaterial<DefaultDecalMat> { RMAT_DEF(BuiltinShader::Decal); };

    /** Determines how is decal blended with the underlying surface. */
    enum class DecalBlendMode
    {
        /** All decal textures are blended with the underlying surface, using alpha to determine blend amount. */
        Transparent,
        /** Albedo texture is multiplied with the underlying surface albedo, while all other textures are blended. */
        Stain,
        /** Only the normal texture is blended with the underlying surface. */
        Normal,
        /** Adds light contribution directly, without writing any other surface data. */
        Emissive
    };

    /** Contains information required for rendering a single Decal. */
    class DecalRenderElement : public RenderElement
    {
    public:
        // TODO

        /** @copydoc RenderElement::Draw */
        void Draw() const override;
    };

    /** Contains information about a Decal, used by the Renderer. */
    struct RendererDecal
    {
        RendererDecal();

        /** Updates the per-object GPU buffer according to the currently set properties. */
        void UpdatePerObjectBuffer();

        /**
         * Updates the per-call GPU buffer according to the provided parameters.
         *
         * @param[in]	viewProj	Combined view-projection matrix of the current camera.
         * @param[in]	flush		True if the buffer contents should be immediately flushed to the GPU.
         */
        void UpdatePerCallBuffer(const Matrix4& viewProj, bool flush = true) const;

        Decal* DecalPtr;
        mutable DecalRenderElement Element;

        SPtr<GpuParamBlockBuffer> DecalParamBuffer;
        SPtr<GpuParamBlockBuffer> PerObjectParamBuffer;
        SPtr<GpuParamBlockBuffer> PerCallParamBuffer;
    };
}
