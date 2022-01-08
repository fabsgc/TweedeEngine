#pragma once

#include "TeRenderManPrerequisites.h"
#include "Renderer/TeRenderable.h"
#include "Renderer/TeRenderElement.h"

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
         * Update the provided instance buffer
         * 
         *  @param[in]	perObjectBuffer	    Per object Buffer which will be filled with data
         *  @param[in]	perInstanceBuffer	Per instance Buffer which will be filled with data
         *  @param[in]	instanceData	    data list we want to store inside instance buffer
         */
        static void UpdatePerInstance(SPtr<GpuParamBlockBuffer>& perObjectBuffer,
            SPtr<GpuParamBlockBuffer>& perInstanceBuffer, PerInstanceData* instanceData, UINT32 instanceCounter);

        /**
         * Update the provided material buffer
         *
         *  @param[in]	perMaterialBuffer	    Per object Buffer which will be filled with data
         *  @param[in]	properties              Material properties
         */
        static void UpdatePerMaterial(SPtr<GpuParamBlockBuffer>& perMaterialBuffer, const MaterialProperties& properties);

        /*
         * Create MaterialData based on MaterialProperties
         *
         * @param[in]	properties material properties
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
        /* In case we are doing instancing, materialBuffer is not needed, do not create it will save memory allocation */
        RenderableElement(bool createPerMaterialBuffer = true);

        /** @copydoc RenderElement::Draw */
        void Draw() const override;

        UINT64 AnimationId = 0;
        RenderableAnimType AnimType = RenderableAnimType::None;
        SPtr<GpuBuffer> BoneMatrixBuffer;
        SPtr<GpuBuffer> BonePrevMatrixBuffer;
    };

    /** Contains information about a Renderable, used by the Renderer. */
    struct RendererRenderable
    {
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

        Matrix4 WorldTfrm = Matrix4::IDENTITY;
        Matrix4 PrevWorldTfrm = Matrix4::IDENTITY;
        PrevFrameDirtyState PreviousFrameDirtyState = PrevFrameDirtyState::Clean;

        Renderable* RenderablePtr;
        Vector<RenderableElement> Elements;

        SPtr<GpuParamBlockBuffer> PerObjectParamBuffer;
    };
}
