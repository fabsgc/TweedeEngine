#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeParamBlocks.h"
#include "Renderer/TeRendererMaterial.h"
#include "PostProcessing/TeGaussianBlurMat.h"

#define STANDARD_MAX_BLUR_SAMPLES 128

namespace te
{
    TE_PARAM_BLOCK_BEGIN(BloomParamDef)
        TE_PARAM_BLOCK_ENTRY(float, gIntensity)
        TE_PARAM_BLOCK_ENTRY(UINT32, gMSAACount)
    TE_PARAM_BLOCK_END

    extern BloomParamDef gBloomParamDef;

    /** Shader that performs Fast Approximate anti-aliasing. */
    class BloomMat : public RendererMaterial<BloomMat>
    {
        RMAT_DEF(BuiltinShader::Bloom);

    public:
        BloomMat();

        /** @copydoc RendererMaterialBase::Initialize */
        void Initialize() override;

        /**
         * Renders the post-process effect with the provided parameters.
         *
         * @param[in]	source		Input texture to apply ToneMappingMat to.
         * @param[in]	destination	Output target to which to write the antialiased image to.
         * @param[in]	emissive	emissive buffer created during first pass
         * @param[in]	MSAACount	how many samples used for input and output textures
         */
        void Execute(const SPtr<Texture>& source, const SPtr<RenderTarget>& destination, const SPtr<Texture>& emissive, 
            const float& intensity = 1.0f, UINT32 MSAACount = 1);

    private:
        SPtr<GpuParamBlockBuffer> _paramBuffer;
    };
}
