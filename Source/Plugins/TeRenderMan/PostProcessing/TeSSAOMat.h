#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeRendererMaterial.h"
#include "Renderer/TeParamBlocks.h"
#include "../TeRendererView.h"

namespace te
{
    TE_PARAM_BLOCK_BEGIN(SSAOParamDef)
        TE_PARAM_BLOCK_ENTRY(float, gSampleRadius)
        TE_PARAM_BLOCK_ENTRY(float, gWorldSpaceRadiusMask)
        TE_PARAM_BLOCK_ENTRY(Vector2, gTanHalfFOV)
        TE_PARAM_BLOCK_ENTRY(Vector2, gRandomTileScale)
        TE_PARAM_BLOCK_ENTRY(float, gCotHalfFOV)
        TE_PARAM_BLOCK_ENTRY(float, gBias)
        TE_PARAM_BLOCK_ENTRY(Vector2, gDownsampledPixelSize)
        TE_PARAM_BLOCK_ENTRY(Vector2, gFadeMultiplyAdd)
        TE_PARAM_BLOCK_ENTRY(float, gPower)
        TE_PARAM_BLOCK_ENTRY(float, gIntensity)
        TE_PARAM_BLOCK_ENTRY(UINT32, gUpSample)
        TE_PARAM_BLOCK_ENTRY(UINT32, gFinalPass)
        TE_PARAM_BLOCK_ENTRY(UINT32, gQuality)
        TE_PARAM_BLOCK_ENTRY(UINT32, gPadding)
    TE_PARAM_BLOCK_END

    extern SSAOParamDef gSSAOParamDef;

    /** Textures used as input when calculating SSAO. */
    struct SSAOTextureInputs
    {
        /** Full resolution scene depth. Only used by final SSAO pass. */
        SPtr<Texture> SceneDepth;

        /** Full resolution buffer containing scene normals. Only used by final SSAO pass. */
        SPtr<Texture> SceneNormals;

        /** Precalculated texture containing downsampled normals/depth, to be used for AO input. */
        SPtr<Texture> AOSetup;

        /** Texture containing AO from the previous pass. Only used if upsampling is enabled. */
        SPtr<Texture> AODownsampled;

        /** Tileable texture containing random rotations that will be applied to AO samples. */
        SPtr<Texture> RandomRotations;
    };

    /** 
     * Shader that computes ambient occlusion using screen based methods.
     */
    class SSAOMat : public RendererMaterial<SSAOMat>
    {
        RMAT_DEF(BuiltinShader::SSAO);

    public:
        SSAOMat();

        /** @copydoc RendererMaterialBase::Initialize */
        void Initialize() override;

        /**
         * Renders the post-process effect with the provided parameters.
         *
         * @param[in]	view			Information about the view we're rendering from.
         * @param[in]	textures		Set of textures to be used as input. Which textures are used depends on the
         *								template parameters of this class.
         * @param[in]	destination		Output texture to which to write the ambient occlusion data to.
         * @param[in]	settings		Settings used to control the ambient occlusion effect.
         * @param[in]	upSample		If true the shader will blend the calculated AO with AO data from the previous pass.
         * @param[in]	finalPass		If true the shader will use the full screen normal/depth information and perform
         *								intensity scaling, as well as distance fade. Otherwise the shader will use the
         *								downsampled AO setup information, with no scaling/fade.
         * @param[in]	quality			Integer in range [0, 4] that controls the quality of SSAO sampling. Higher numbers yield
         *								better quality at the cost of performance.
         */
        void Execute(const RendererView& view, const SSAOTextureInputs& textures, const SPtr<RenderTexture>& destination,
            const AmbientOcclusionSettings& settings, bool upSample, bool finalPass, UINT32 quality);

    private:
        SPtr<GpuParamBlockBuffer> _paramBuffer;
    };
}
