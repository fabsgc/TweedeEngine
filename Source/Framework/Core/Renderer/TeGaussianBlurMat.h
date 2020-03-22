#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeRendererMaterial.h"
#include "Renderer/TeParamBlocks.h"
#include "Math/TeVector4.h"
#include "RenderAPI/TeRenderTexture.h"

#define STANDARD_MAX_BLUR_SAMPLES 128

namespace te
{
    TE_PARAM_BLOCK_BEGIN(GaussianBlurParamDef)
        TE_PARAM_BLOCK_ENTRY_ARRAY(Vector4, gSampleOffsets, (STANDARD_MAX_BLUR_SAMPLES + 1) / 2)
        TE_PARAM_BLOCK_ENTRY_ARRAY(Vector4, gSampleWeights, STANDARD_MAX_BLUR_SAMPLES)
        TE_PARAM_BLOCK_ENTRY(UINT32, gNumSamples)
        TE_PARAM_BLOCK_ENTRY(UINT32, gMSAACount)
    TE_PARAM_BLOCK_END

    extern GaussianBlurParamDef gGaussianBlurParamDef;

    /** Shader that performs Fast Approximate anti-aliasing. */
    class TE_CORE_EXPORT GaussianBlurMat : public RendererMaterial<GaussianBlurMat>
    {
        RMAT_DEF(BuiltinShader::GaussianBlur);

    public:
        // Direction of the Gaussian filter pass
        enum class Direction
        {
            DirVertical,
            DirHorizontal
        };

        GaussianBlurMat();

        /**
         * Renders the post-process effect with the provided parameters.
         *
         * @param[in]	source		Input texture to blur.
         * @param[in]	filterSize	Size of the blurring filter, in percent of the source texture. In range [0, 1].
         * @param[in]	destination	Output texture to which to write the blurred image to.
         * @param[in]	tint		Optional tint to apply all filtered pixels.
         * @param[in]	additive	Optional texture whose values to add to the destination texture (won't be included
         *							in filtering). Only used if using the variation of this shader that supports additive
         *							input.
         */
        void Execute(const SPtr<Texture>& source, const SPtr<RenderTexture>& destination, float filterSize, 
            const Color& tint = Color::White, const SPtr<Texture>& additive = nullptr, UINT32 MSAACount = 1);

        /**
         * Populates the provided parameter buffer with parameters required for a shader including gaussian blur.
         *
         * @param[in]	buffer		Buffer to write the parameters to. Must be created using @p GaussianBlurParamDef.
         * @param[in]	direction	Direction in which to perform the separable blur.
         * @param[in]	source		Source texture that needs to be blurred.
         * @param[in]	filterSize	Size of the blurring filter, in percent of the source texture. In range [0, 1].
         * @param[in]	tint		Optional tint to apply all filtered pixels.
         */
        static void PopulateBuffer(const SPtr<GpuParamBlockBuffer>& buffer, Direction direction,
            const SPtr<Texture>& source, float filterSize, const Color& tint = Color::White);

    private:
        /** Calculates weights and offsets for the standard distribution of the specified filter size. */
        static UINT32 CalcStdDistribution(float filterRadius, std::array<float, STANDARD_MAX_BLUR_SAMPLES>& weights,
            std::array<float, STANDARD_MAX_BLUR_SAMPLES>& offsets);

        /** Calculates the radius of the blur kernel depending on the source texture size and provided scale. */
        static float CalcKernelRadius(const SPtr<Texture>& source, float scale, Direction filterDir);

    private:
        SPtr<GpuParamBlockBuffer> _paramBuffer;
        SPtr<Texture> _inputTexture;
        SPtr<Texture> _additiveTexture;
        bool _isAdditive = false;
    };
}
