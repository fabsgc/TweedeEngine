#pragma once

#include "TeRenderManPrerequisites.h"
#include "Renderer/TeRenderable.h"
#include "Renderer/TeParamBlocks.h"
#include "Renderer/TeRenderElement.h"
#include "Mesh/TeMesh.h"

#define STANDARD_FORWARD_MAX_INSTANCED_BLOCK 128

namespace te
{
    struct PerInstanceData
    {
        Matrix4 gMatWorld;
        Matrix4 gMatInvWorld;
        Matrix4 gMatWorldNoScale;
        Matrix4 gMatInvWorldNoScale;
        Matrix4 gMatPrevWorld;
        INT32   gLayer;
    };

    TE_PARAM_BLOCK_BEGIN(PerObjectParamDef)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatWorld)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatInvWorld)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatWorldNoScale)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatInvWorldNoScale)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatPrevWorld)
        TE_PARAM_BLOCK_ENTRY(INT32, gLayer)
        TE_PARAM_BLOCK_ENTRY(INT32, gInstanced) // default 0 for non instanced object
    TE_PARAM_BLOCK_END

    extern PerObjectParamDef gPerObjectParamDef;

    TE_PARAM_BLOCK_BEGIN(PerCallParamDef)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatWorldViewProj)
    TE_PARAM_BLOCK_END

    extern PerCallParamDef gPerCallParamDef;

    TE_PARAM_BLOCK_BEGIN(PerInstanceParamDef)
        TE_PARAM_BLOCK_ENTRY_ARRAY(PerInstanceData, gInstances, STANDARD_FORWARD_MAX_INSTANCED_BLOCK)
    TE_PARAM_BLOCK_END

    extern PerInstanceParamDef gPerInstanceParamDef;

    /** Helper class used for manipulating the PerObject parameter buffer. */
    class PerObjectBuffer
    {
    public:
        /** Updates the provided buffer with the data from the provided matrices. */
        static void Update(SPtr<GpuParamBlockBuffer>& buffer, const Matrix4& tfrm, const Matrix4& tfrmNoScale, 
            const Matrix4& prevTfrm, UINT32 layer);

        /** Update the provided instance buffer and set per object buffer to enable instancing for this object */
        static void UpdatePerInstance(SPtr<GpuParamBlockBuffer>& perObjectBuffer,
            SPtr<GpuParamBlockBuffer>& perInstanceBuffer, const Vector<PerInstanceData>& instanceData);
    };

    /**
     * Contains information required for rendering a single Renderable sub-mesh, representing a generic static or animated
     * 3D model.
     */
    class RenderableElement : public RenderElement
    {
    public:
        /** @copydoc RenderElement::UpdateGpuParams */
        void UpdateGpuParams() const override;

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

        /** Updates the per-instance GPU buffer according to the currently set properties. */
        void UpdatePerInstanceBuffer(Vector<PerInstanceData>& instanceData);

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
        SPtr<GpuParamBlockBuffer> PerInstanceParamBuffer;
        SPtr<GpuParamBlockBuffer> PerCallParamBuffer;
    };
}
