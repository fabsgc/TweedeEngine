#pragma once

#include "TeCorePrerequisites.h"
#include "RenderAPI/TeRenderTexture.h"
#include "Renderer/TeRendererMaterial.h"
#include "Renderer/TeParamBlocks.h"

#define STANDARD_MAX_BLUR_SAMPLES 128

namespace te
{
    TE_PARAM_BLOCK_BEGIN(GaussianBlurParamDef)
        TE_PARAM_BLOCK_ENTRY(Vector2, gSourceDimensions)
        TE_PARAM_BLOCK_ENTRY(UINT32, gMSAACount)
        TE_PARAM_BLOCK_ENTRY(UINT32, gHorizontal)
        TE_PARAM_BLOCK_ENTRY(UINT32, gNumSamples)
    TE_PARAM_BLOCK_END

    extern GaussianBlurParamDef gGaussianBlurParamDef;

    /** Shader that performs Fast Approximate anti-aliasing. */
    class TE_CORE_EXPORT GaussianBlurMat : public RendererMaterial<GaussianBlurMat>
    {
        RMAT_DEF(BuiltinShader::GaussianBlur);

    public:
        GaussianBlurMat();

        /**
         * Renders the post-process effect with the provided parameters.
         *
         * @param[in]	source		Input texture to blur.
         * @param[in]	destination	Output texture to which to write the blurred image to.
         * @param[in]	numSamples	How many samples we can use. Default is 7 (Horizontally and Vertically).
         * @param[in]	MSAACount   Specify if input texture is multisampled
         */
        void Execute(const SPtr<Texture>& source, const SPtr<RenderTexture>& destination, UINT32 numSamples = 7, UINT32 MSAACount = 1);

    private:
        void DoPass(bool horizontal, const SPtr<Texture>& source, const SPtr<RenderTexture>& destination, UINT32 MSAACount = 1);

    private:
        SPtr<GpuParamBlockBuffer> _paramBuffer;
        SPtr<Texture> _inputTexture;
    };
}
