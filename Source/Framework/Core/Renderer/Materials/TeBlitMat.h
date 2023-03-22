#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeRendererMaterial.h"
#include "Renderer/TeParamBlocks.h"

namespace te
{
    /** 
     * Shader that copies a source texture into a render target, and optionally resolves it. 
     */
    class TE_CORE_EXPORT BlitMat : public RendererMaterial<BlitMat>
    {
        RMAT_DEF(BuiltinShader::Blit);

        /** Helper method used for initializing variations of this material. */
        template<UINT32 MSAA, UINT32 MODE>
        static const ShaderVariation& GetVariation()
        {
            static ShaderVariation variation = ShaderVariation(
            Vector<ShaderVariation::Param>({
                ShaderVariation::Param("MSAA_COUNT", MSAA),
                ShaderVariation::Param("MODE", MODE)
            }));

            return variation;
        }

    public:
        BlitMat();

        /** @copydoc RendererMaterialBase::Initialize */
        void Initialize() override;

        /** Executes the material on the currently bound render target, copying from @p source. */
        void Execute(const SPtr<Texture>& source, const Rect2& area, bool flipUV);

        /**
         * Returns the material variation matching the provided parameters.
         *
         * @param	msaaCount		Number of MSAA samples in the input texture. If larger than 1 the texture will be resolved
         *							before written to the destination.
         * @param	isColor			If true the input is assumed to be a 4-component color texture. If false it is assumed
         *							the input is a 1-component depth texture. This controls how is the texture resolve and is
         *							only relevant if @p msaaCount > 1. Color texture MSAA samples will be averaged, while for
         *							depth textures the minimum of all samples will be used.
         */
        static BlitMat* GetVariation(UINT32 msaaCount, bool isColor);
    private:
        SPtr<GpuParamBlockBuffer> _paramBuffer;
    };
}
