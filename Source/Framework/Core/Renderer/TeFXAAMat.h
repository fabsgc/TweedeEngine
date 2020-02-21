#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeRendererMaterial.h"
#include "Renderer/TeParamBlocks.h"

namespace te
{
    TE_PARAM_BLOCK_BEGIN(FXAAParamDef)
        TE_PARAM_BLOCK_ENTRY(Vector2, gInvTexSize)
    TE_PARAM_BLOCK_END

    extern FXAAParamDef gFXAAParamDef;

    /** Shader that performs Fast Approximate anti-aliasing. */
    class TE_CORE_EXPORT FXAAMat : public RendererMaterial<FXAAMat>
    {
        RMAT_DEF(BuiltinShader::FXAA);

    public:
        FXAAMat();

        /**
         * Renders the post-process effect with the provided parameters.
         *
         * @param[in]	source		Input texture to apply FXAA to.
         * @param[in]	destination	Output target to which to write the antialiased image to.
         */
        void Execute(const SPtr<Texture>& source, const SPtr<RenderTarget>& destination);

    private:
        SPtr<GpuParamBlockBuffer> _paramBuffer;
    };
}
