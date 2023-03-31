#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeRendererMaterial.h"
#include "Renderer/TeParamBlocks.h"

namespace te
{
    TE_PARAM_BLOCK_BEGIN(ToneMappingParamDef)
        TE_PARAM_BLOCK_ENTRY(float, gGamma)
        TE_PARAM_BLOCK_ENTRY(float, gExposure)
        TE_PARAM_BLOCK_ENTRY(float, gContrast)
        TE_PARAM_BLOCK_ENTRY(float, gBrightness)
    TE_PARAM_BLOCK_END

    extern ToneMappingParamDef gToneMappingParamDef;

    /** Shader that performs Fast Approximate anti-aliasing. */
    class ToneMappingMat : public RendererMaterial<ToneMappingMat>
    {
        RMAT_DEF(BuiltinShader::ToneMapping);

        /** Helper method used for initializing variations of this material. */
        template<UINT32 MSAA, bool GAMMA_ONLY>
        static const ShaderVariation& GetVariation()
        {
            static ShaderVariation variation = ShaderVariation(
            Vector<ShaderVariation::Param>({
                ShaderVariation::Param("MSAA_COUNT", MSAA),
                ShaderVariation::Param("GAMMA_ONLY", GAMMA_ONLY)
            }));

            return variation;
        }

    public:
        ToneMappingMat();

        /** @copydoc RendererMaterialBase::Initialize */
        void Initialize() override;

        /**
         * Renders the post-process effect with the provided parameters.
         *
         * @param[in]	source		Input texture to apply ToneMappingMat to.
         * @param[in]	destination	Output target to which to write the antialiased image to.
         * @param[in]	MSAACount 
         * @param[in]	gamma 
         * @param[in]	exposure 
         * @param[in]	contrast 
         * @param[in]	brightness 
         * @param[in]	gammaOnly 
         */
        void Execute(const SPtr<Texture>& ssao, const SPtr<Texture>& source, const SPtr<RenderTarget>& destination,
            INT32 MSAACount = 1, float gamma = 2.2f, float exposure = 1.0f, float contrast = 1.0f, float brightness = 0.0f, bool gammaOnly = false);

        /**
         * Returns the material variation matching the provided parameters.
         *
         * @param	msaaCount		Number of MSAA samples in the input texture. If larger than 1 the texture will be resolved
         *							before written to the destination.
         * @param	gammaOnly
         */
        static ToneMappingMat* GetVariation(UINT32 msaaCount, bool gammaOnly);

    private:
        SPtr<GpuParamBlockBuffer> _paramBuffer;
    };
}
