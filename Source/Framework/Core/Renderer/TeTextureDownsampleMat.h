#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeRendererMaterial.h"
#include "Renderer/TeParamBlocks.h"
#include "Renderer/TeGpuResourcePool.h"

namespace te
{
    TE_PARAM_BLOCK_BEGIN(TextureDownsampleParamDef)
        TE_PARAM_BLOCK_ENTRY(UINT32, gMipLevel)
        TE_PARAM_BLOCK_ENTRY(Vector2, gPadding)
    TE_PARAM_BLOCK_END

    extern TextureDownsampleParamDef gTextureDownsampleParamDef;

    /** 
     * Shader that copies a source texture into a render target, and optionally resolves it. 
     */
    class TE_CORE_EXPORT TextureDownsampleMat : public RendererMaterial<TextureDownsampleMat>
    {
        RMAT_DEF(BuiltinShader::TextureDownsample)

    public:
        TextureDownsampleMat();

        /** @copydoc RendererMaterialBase::Initialize */
        void Initialize() override;

        /** Downsamples the provided texture face and outputs it to the provided target. */
        void Execute(const SPtr<Texture>& source, UINT32 mip, const SPtr<RenderTarget>& target);

    private:
        SPtr<GpuParamBlockBuffer> _paramBuffer;
    };
}
