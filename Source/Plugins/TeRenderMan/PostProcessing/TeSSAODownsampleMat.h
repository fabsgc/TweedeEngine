#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeRendererMaterial.h"
#include "Renderer/TeParamBlocks.h"
#include "../TeRendererView.h"

namespace te
{
    TE_PARAM_BLOCK_BEGIN(SSAODownsampleParamDef)
        TE_PARAM_BLOCK_ENTRY(Vector2, gPixelSize)
        TE_PARAM_BLOCK_ENTRY(float, gInvDepthThreshold)
    TE_PARAM_BLOCK_END

    extern SSAODownsampleParamDef gSSAODownsampleParamDef;

    /** 
     * Shader that downsamples the depth & normal buffer and stores their results in a common texture, to be consumed
     * by SSAOMat.
     */
    class SSAODownsampleMat : public RendererMaterial<SSAODownsampleMat>
    {
        RMAT_DEF(BuiltinShader::SSAODownSample);

    public:
        SSAODownsampleMat();

        /** @copydoc RendererMaterialBase::Initialize */
        void Initialize() override;

        /**
         * Renders the post-process effect with the provided parameters.
         *
         * @param[in]	view			Information about the view we're rendering from.
         * @param[in]	sceneDepth		Input texture containing scene depth.
         * @param[in]	sceneNormals	Input texture containing scene world space normals.
         * @param[in]	destination		Output texture to which to write the downsampled data to.
         * @param[in]	depthRange		Valid depth range (in view space) within which nearby samples will be averaged.
         */
        void Execute(const RendererView& view, const SPtr<Texture>& sceneDepth, const SPtr<Texture>& sceneNormals,
            const SPtr<RenderTexture>& destination, float depthRange);

    private:
        SPtr<GpuParamBlockBuffer> _paramBuffer;
    };
}
