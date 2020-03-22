#pragma once

#include "TeCorePrerequisites.h"
#include "Math/TeRect2.h"
#include "Math/TeVector2I.h"
#include "Renderer/TeRendererMaterial.h"
#include "Renderer/TeParamBlocks.h"

namespace te
{
    TE_PARAM_BLOCK_BEGIN(BlitParamDef)
        TE_PARAM_BLOCK_ENTRY(INT32, gMSAACount)
        TE_PARAM_BLOCK_ENTRY(INT32, gIsDepth)
    TE_PARAM_BLOCK_END

    extern BlitParamDef gBlitParamDef;

    /** 
     * Shader that copies a source texture into a render target, and optionally resolves it. 
     */
    class TE_CORE_EXPORT BlitMat : public RendererMaterial<BlitMat>
    {
        RMAT_DEF(BuiltinShader::Blit);

    public:
        BlitMat();

        /** Executes the material on the currently bound render target, copying from @p source. */
        void Execute(const SPtr<Texture>& source, const Rect2& area, bool flipUV, INT32 MSAACount = 1, bool isDepth = false);

    private:
        SPtr<GpuParamBlockBuffer> _paramBuffer;
    };
}
