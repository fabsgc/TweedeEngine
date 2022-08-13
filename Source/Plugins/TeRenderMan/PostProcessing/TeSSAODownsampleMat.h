#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeRendererMaterial.h"
#include "Renderer/TeParamBlocks.h"

namespace te
{
    TE_PARAM_BLOCK_BEGIN(SSAODownsampleParamDef)
    TE_PARAM_BLOCK_END

    extern SSAODownsampleParamDef gSSAODownsampleParamDef;

    /** 
     * Shader that renders a SSAODownsample using a cubemap or a solid color. 
     */
    class SSAODownsampleMat : public RendererMaterial<SSAODownsampleMat>
    {
        RMAT_DEF(BuiltinShader::SSAODownSample);

    public:
        SSAODownsampleMat();

        /** @copydoc RendererMaterialBase::Initialize */
        void Initialize() override;

        /** Binds the material for rendering and sets up any parameters. */
        void Bind();

    private:
        SPtr<GpuParamBlockBuffer> _paramBuffer;
    };
}
