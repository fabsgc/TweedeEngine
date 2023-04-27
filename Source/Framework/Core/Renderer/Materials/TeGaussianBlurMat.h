#pragma once

#include "TeCorePrerequisites.h"
#include "RenderAPI/TeRenderTexture.h"
#include "Renderer/TeRendererMaterial.h"
#include "Renderer/TeParamBlocks.h"

#define STANDARD_MAX_BLUR_SAMPLES 128

namespace te
{
    const int MAX_BLUR_SAMPLES = 128;

    TE_PARAM_BLOCK_BEGIN(GaussianBlurParamDef)
        TE_PARAM_BLOCK_ENTRY(Vector2, gSourceDimensions)
        TE_PARAM_BLOCK_ENTRY(UINT32, gNumSamples)
        TE_PARAM_BLOCK_ENTRY(UINT32, gPadding)
        TE_PARAM_BLOCK_ENTRY_ARRAY(Vector4, gSampleOffsets, (MAX_BLUR_SAMPLES + 1) / 2)
        TE_PARAM_BLOCK_ENTRY_ARRAY(Vector4, gSampleWeights, MAX_BLUR_SAMPLES)
    TE_PARAM_BLOCK_END

    extern GaussianBlurParamDef gGaussianBlurParamDef;

    /** Shader that performs Fast Approximate anti-aliasing. */
    class TE_CORE_EXPORT GaussianBlurMat : public RendererMaterial<GaussianBlurMat>
    {
        RMAT_DEF(BuiltinShader::GaussianBlur);

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
        GaussianBlurMat();

        /** @copydoc RendererMaterialBase::Initialize */
        void Initialize() override;

        /**
         * Renders the post-process effect with the provided parameters.
         *
         * @param[in]	source		Input texture to blur.
         * @param[in]	destination	Output texture to which to write the blurred image to.
         * @param[in]	filterSize	Size of the blurring filter, in percent of the source texture. In range [0, 1].
         * @param[in]	maxSamples  max samples to use
         * @param[in]	tint		Optional tint to apply all filtered pixels.
         * @param[in]	MSAACount   Specify if input texture is multisampled
         
         */
        void Execute(const SPtr<Texture>& source, const SPtr<RenderTexture>& destination, float filterSize, UINT32 maxSamples = MAX_BLUR_SAMPLES, 
            const Color& tint = Color::White, UINT32 MSAACount = 1);

        /**
         * Populates the provided parameter buffer with parameters required for a shader including gaussian blur.
         *
         * @param[in]	buffer		Buffer to write the parameters to. Must be created using @p GaussianBlurParamDef.
         * @param[in]	horizontal	Direction in which to perform the separable blur.
         * @param[in]	source		Source texture that needs to be blurred.
         * @param[in]	filterSize	Size of the blurring filter, in percent of the source texture. In range [0, 1].
         * @param[in]	maxSamples  max samples to use
         * @param[in]	tint		Optional tint to apply all filtered pixels.
         */
        static void PopulateBuffer(const SPtr<GpuParamBlockBuffer>& buffer, bool horizontal,
            const SPtr<Texture>& source, float filterSize, UINT32 maxSamples, const Color& tint = Color::White);

        /**
         * Returns the material variation matching the provided parameters.
         *
         * @param	msaaCount		Number of MSAA samples in the input texture. If larger than 1 the texture will be resolved
         *							before written to the destination.
         */
        static GaussianBlurMat* GetVariation(UINT32 msaaCount);

    private:
        /** Calculates weights and offsets for the standard distribution of the specified filter size. */
        static UINT32 CalcStdDistribution(float filterRadius, UINT32 maxSamples, std::array<float, MAX_BLUR_SAMPLES>& weights,
            std::array<float, MAX_BLUR_SAMPLES>& offsets);

        /** Calculates the radius of the blur kernel depending on the source texture size and provided scale. */
        static float CalcKernelRadius(const SPtr<Texture>& source, float scale, UINT32 maxSamples, bool horizontal);

        /** Execute a blur pass (horizontal or vertical) */
        void DoPass(bool horizontal, const SPtr<Texture>& source, const SPtr<RenderTexture>& destination, float filterSize, 
            UINT32 maxSamples, const Color& tint);

    private:
        SPtr<GpuParamBlockBuffer> _paramBuffer;
        SPtr<Texture> _inputTexture;
    };
}
