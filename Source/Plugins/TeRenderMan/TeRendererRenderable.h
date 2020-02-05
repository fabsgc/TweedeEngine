#pragma once

#include "TeRenderManPrerequisites.h"
#include "Renderer/TeRenderable.h"
#include "Renderer/TeParamBlocks.h"
#include "Renderer/TeRenderElement.h"
#include "Mesh/TeMesh.h"

namespace te
{
    TE_PARAM_BLOCK_BEGIN(PerObjectParamDef)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatWorld)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatPrevWorld)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatWorldNoScale)
    TE_PARAM_BLOCK_END

    extern PerObjectParamDef gPerObjectParamDef;

    TE_PARAM_BLOCK_BEGIN(PerCallParamDef)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatWorldViewProj)
    TE_PARAM_BLOCK_END

    extern PerCallParamDef gPerCallParamDef;

    /** Helper class used for manipulating the PerObject parameter buffer. */
    class PerObjectBuffer
    {
    public:
        /** Updates the provided buffer with the data from the provided matrices. */
        static void Update(SPtr<GpuParamBlockBuffer>& buffer, const Matrix4& tfrm, const Matrix4& tfrmNoScale, const Matrix4& prevTfrm);
    };

    /**
     * Contains information required for rendering a single Renderable sub-mesh, representing a generic static or animated
     * 3D model.
     */
    class RenderableElement : public RenderElement
    {
    public:
        /** @copydoc RenderElement::Draw */
        void Draw() const override;
    };

    /** Contains information about a Renderable, used by the Renderer. */
    struct RendererRenderable
    {
    public:
        RendererRenderable();
        ~RendererRenderable();

        /** Updates the per-object GPU buffer according to the currently set properties. */
        void UpdatePerObjectBuffer();

        /**
         * Updates the per-call GPU buffer according to the provided parameters.
         *
         * @param[in]	viewProj	Combined view-projection matrix of the current camera.
         * @param[in]	flush		True if the buffer contents should be immediately flushed to the GPU.
         */
        void UpdatePerCallBuffer(const Matrix4& viewProj, bool flush = true);

        Matrix4 WorldTfrm = Matrix4::IDENTITY;
        Matrix4 PrevWorldTfrm = Matrix4::IDENTITY;
        PrevFrameDirtyState PreviousFrameDirtyState = PrevFrameDirtyState::Clean;

        Renderable* RenderablePtr;
        Vector<RenderableElement> Elements;

        SPtr<GpuParamBlockBuffer> PerObjectParamBuffer;
        SPtr<GpuParamBlockBuffer> PerCallParamBuffer;
    };
}
