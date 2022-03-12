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
        TE_PARAM_BLOCK_ENTRY(UINT32, gMSAACount)
        TE_PARAM_BLOCK_ENTRY(UINT32, gHorizontal)
        TE_PARAM_BLOCK_ENTRY(UINT32, gNumSamples)
        TE_PARAM_BLOCK_ENTRY(Vector3, gPadding)
        TE_PARAM_BLOCK_ENTRY_ARRAY(Vector4, gSampleOffsets, (MAX_BLUR_SAMPLES + 1) / 2)
        TE_PARAM_BLOCK_ENTRY_ARRAY(Vector4, gSampleWeights, MAX_BLUR_SAMPLES)
    TE_PARAM_BLOCK_END

    extern GaussianBlurParamDef gGaussianBlurParamDef;

    /** Shader that performs Fast Approximate anti-aliasing. */
    class GaussianBlurMat : public RendererMaterial<GaussianBlurMat>
    {
        RMAT_DEF(BuiltinShader::GaussianBlur);

    public:
        GaussianBlurMat();

        /**
         * Renders the post-process effect with the provided parameters.
         *
         * @param[in]	source		Input texture to blur.
         * @param[in]	destination	Output texture to which to write the blurred image to.
         * @param[in]	filterSize	Size of the blurring filter, in percent of the source texture. In range [0, 1].
         * @param[in]	tint		Optional tint to apply all filtered pixels.
         * @param[in]	MSAACount   Specify if input texture is multisampled
         */
        void Execute(const SPtr<Texture>& source, const SPtr<RenderTexture>& destination, float filterSize, const Color& tint = Color::White, UINT32 MSAACount = 1);
            
        /**
         * Populates the provided parameter buffer with parameters required for a shader including gaussian blur.
         *
         * @param[in]	buffer		Buffer to write the parameters to. Must be created using @p GaussianBlurParamDef.
         * @param[in]	horizontal	Direction in which to perform the separable blur.
         * @param[in]	source		Source texture that needs to be blurred.
         * @param[in]	filterSize	Size of the blurring filter, in percent of the source texture. In range [0, 1].
         * @param[in]	tint		Optional tint to apply all filtered pixels.
         */
        static void PopulateBuffer(const SPtr<GpuParamBlockBuffer>& buffer, bool horizontal,
            const SPtr<Texture>& source, float filterSize, const Color& tint = Color::White);

    private:
        /** Calculates weights and offsets for the standard distribution of the specified filter size. */
        static UINT32 CalcStdDistribution(float filterRadius, std::array<float, MAX_BLUR_SAMPLES>& weights,
            std::array<float, MAX_BLUR_SAMPLES>& offsets);

        /** Calculates the radius of the blur kernel depending on the source texture size and provided scale. */
        static float CalcKernelRadius(const SPtr<Texture>& source, float scale, bool horizontal);

        /** Execute a blur pass (horizontal or vertical) */
        void DoPass(bool horizontal, const SPtr<Texture>& source, const SPtr<RenderTexture>& destination, float filterSize, const Color& tint, UINT32 MSAACount = 1);

    private:
        SPtr<GpuParamBlockBuffer> _paramBuffer;
        SPtr<Texture> _inputTexture;
    };
}
