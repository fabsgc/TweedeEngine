#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeParamBlocks.h"
#include "Renderer/TeRendererMaterial.h"

#define STANDARD_MAX_BLUR_SAMPLES 128

namespace te
{
    TE_PARAM_BLOCK_BEGIN(BloomParamDef)
        TE_PARAM_BLOCK_ENTRY(float, gIntensity)
    TE_PARAM_BLOCK_END

    extern BloomParamDef gBloomParamDef;

    /** Shader that performs Fast Approximate anti-aliasing. */
    class BloomMat : public RendererMaterial<BloomMat>
    {
        RMAT_DEF(BuiltinShader::Bloom);

        /** Helper method used for initializing variations of this material. */
        template<UINT32 MSAA>
        static const ShaderVariation& GetVariation()
        {
            static ShaderVariation variation = ShaderVariation(
                Vector<ShaderVariation::Param>({
                    ShaderVariation::Param("MSAA_COUNT", MSAA),
                }));

            return variation;
        }

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
         */
        void Execute(const SPtr<Texture>& source, const SPtr<RenderTarget>& destination, const SPtr<Texture>& emissive, 
            const float& intensity = 1.0f);

        /**
         * Returns the material variation matching the provided parameters.
         *
         * @param	msaaCount		Number of MSAA samples in the input texture. If larger than 1 the texture will be resolved
         *							before written to the destination.
         */
        static BloomMat* GetVariation(UINT32 msaaCount);

    private:
        SPtr<GpuParamBlockBuffer> _paramBuffer;
    };
}
