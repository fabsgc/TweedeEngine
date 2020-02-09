#pragma once

#include "TeRenderManPrerequisites.h"
#include "Renderer/TeRenderable.h"
#include "Renderer/TeParamBlocks.h"
#include "Renderer/TeRenderElement.h"
#include "Mesh/TeMesh.h"

namespace te
{
    /** Helper class used for manipulating the PerObject parameter buffer. */
    class PerObjectBuffer
    {
    public:
        /** 
         * Updates the provided buffer with the data from the provided matrices. 
         *
         *  @param[in]	buffer	      Buffer which will be filled with data
         *  @param[in]	tfrm	      World matrix of current object
         *  @param[in]	prevTfrm	  Previous World matrix of current object
         *  @param[in]	RenderablePtr Pointer to the current Renderable we want to update
         */
        static void UpdatePerObject(SPtr<GpuParamBlockBuffer>& buffer, const Matrix4& tfrm,
            const Matrix4& prevTfrm, Renderable* RenderablePtr);

        /** 
         * Update the provided instance buffer and set per object buffer to enable instancing for this object 
         * 
         *  @param[in]	perObjectBuffer	    Per object Buffer which will be filled with data
         *  @param[in]	perInstanceBuffer	Per instance Buffer which will be filled with data
         *  @param[in]	instanceData	    data list we want to store inside instance buffer
         */
        static void UpdatePerInstance(SPtr<GpuParamBlockBuffer>& perObjectBuffer,
            SPtr<GpuParamBlockBuffer>& perInstanceBuffer, PerInstanceData* instanceData, UINT32 instanceCounter);

        /*
         * Create MaterialData based on MaterialProperties
         */
        static MaterialData ConvertMaterialProperties(const MaterialProperties& properties);
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
         * Updates the per-instance GPU buffer according to the currently set properties. 
         *
         * @param[in]	instanceData	Array with max with of 64
         * @param[in]	instanceCounter	Number of valid element to store in array
         * @param[in]	blockId	        We create only 8 instance buffer, we need to know in which we want to register data
         */
        void UpdatePerInstanceBuffer(PerInstanceData* instanceData, UINT32 instanceCounter, UINT32 blockId);

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
