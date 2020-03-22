#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeRendererMaterial.h"
#include "Renderer/TeParamBlocks.h"
#include "Renderer/TeRenderSettings.h"


namespace te
{
    TE_PARAM_BLOCK_BEGIN(MotionBlurParamDef)
        TE_PARAM_BLOCK_ENTRY(float, gFrameDelta)
        TE_PARAM_BLOCK_ENTRY(INT32, gHalfNumSamples)
        TE_PARAM_BLOCK_ENTRY(UINT32, gMSAACount)
    TE_PARAM_BLOCK_END

    extern MotionBlurParamDef gMotionBlurParamDef;

    /** Shader that performs Fast Approximate anti-aliasing. */
    class TE_CORE_EXPORT MotionBlurMat : public RendererMaterial<MotionBlurMat>
    {
        RMAT_DEF(BuiltinShader::MotionBlur);

    public:
        MotionBlurMat();

        /**
         * Renders the post-process effect with the provided parameters.
         *
         * @param[in]	source			Input texture to apply ToneMappingMat to.
         * @param[in]	destination		Output target to which to write the antialiased image to.
         * @param[in]	depth			Depth buffer created during first pass
         * @param[in]	perViewBuffer	Camera frame buffer
         * @param[in]	settings		Motion blur settings
         * @param[in]	MSAACount		How many samples used for input and output textures
         */
        void Execute(const SPtr<Texture>& source, const SPtr<RenderTarget>& destination, const SPtr<Texture>& depth, 
            SPtr<GpuParamBlockBuffer> perViewBuffer, const MotionBlurSettings& settings, INT32 MSAACount = 1);

    private:
        SPtr<GpuParamBlockBuffer> _paramBuffer;
    };
}
