#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeRendererMaterial.h"
#include "Renderer/TeParamBlocks.h"

namespace te
{
    TE_PARAM_BLOCK_BEGIN(BloomParamDef)
        TE_PARAM_BLOCK_ENTRY(INT32, gMSAACount)
    TE_PARAM_BLOCK_END

    extern BloomParamDef gBloomParamDef;

    /** Shader that performs Fast Approximate anti-aliasing. */
    class TE_CORE_EXPORT BloomMat : public RendererMaterial<BloomMat>
    {
        RMAT_DEF(BuiltinShader::Bloom);

    public:
        BloomMat();

        /**
         * Renders the post-process effect with the provided parameters.
         *
         * @param[in]	source		Input texture to apply ToneMappingMat to.
         * @param[in]	destination	Output target to which to write the antialiased image to.
         * @param[in]	emissive	emissive buffer created during first pass
         * @param[in]	MSSACount	how many samples used for input and output textures
         */
        void Execute(const SPtr<Texture>& source, const SPtr<RenderTarget>& destination, const SPtr<Texture>& emissive, INT32 MSSACount = 1);

    private:
        SPtr<GpuParamBlockBuffer> _paramBuffer;
    };
}
