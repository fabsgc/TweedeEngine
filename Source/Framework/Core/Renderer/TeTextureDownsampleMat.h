#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeRendererMaterial.h"
#include "Renderer/TeParamBlocks.h"
#include "Renderer/TeGpuResourcePool.h"

namespace te
{
    /** 
     * Shader that copies a source texture into a render target, and optionally resolves it. 
     */
    class TE_CORE_EXPORT TextureDownsampleMat : public RendererMaterial<TextureDownsampleMat>
    {
        RMAT_DEF(BuiltinShader::TextureDownsample)

        /** Helper method used for initializing variations of this material. */
        template<UINT32 MSAA>
        static const ShaderVariation& GetVariation()
        {
            static ShaderVariation variation;
            variation = ShaderVariation(
                Vector<ShaderVariation::Param>({
                    ShaderVariation::Param("MSAA_COUNT", MSAA),
                }));

            return variation;
        }

    public:
        TextureDownsampleMat();

        /** @copydoc RendererMaterialBase::Initialize */
        void Initialize() override;

        /** Downsamples the provided texture face and outputs it to the provided target. */
        void Execute(const SPtr<Texture>& source, UINT32 mip, const SPtr<RenderTarget>& target);

        /**
         * Returns the material variation matching the provided parameters.
         *
         * @param	msaaCount		Number of MSAA samples in the input texture. If larger than 1 the texture will be resolved
         *							before written to the destination.
         */
        static TextureDownsampleMat* GetVariation(UINT32 msaaCount);
    };
}
