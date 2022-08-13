#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeRendererMaterial.h"
#include "Renderer/TeParamBlocks.h"
#include "../TeRendererView.h"

namespace te
{
    TE_PARAM_BLOCK_BEGIN(SSAOBlurParamDef)
        TE_PARAM_BLOCK_ENTRY(Vector2, gPixelSize)
        TE_PARAM_BLOCK_ENTRY(Vector2, gPixelOffset)
        TE_PARAM_BLOCK_ENTRY(float, gInvDepthThreshold)
        TE_PARAM_BLOCK_ENTRY(UINT32, gHorizontal)
        TE_PARAM_BLOCK_ENTRY(Vector2, gPadding)
    TE_PARAM_BLOCK_END

    extern SSAOBlurParamDef gSSAOBlurParamDef;

    /**
     * Shaders that blurs the ambient occlusion output, in order to hide the noise caused by the randomization texture.
     */
    class SSAOBlurMat : public RendererMaterial<SSAOBlurMat>
    {
        RMAT_DEF(BuiltinShader::SSAOBlur);

    public:
        SSAOBlurMat();

        /** @copydoc RendererMaterialBase::Initialize */
        void Initialize() override;

        /**
		 * Renders the post-process effect with the provided parameters.
		 *
		 * @param[in]	view			Information about the view we're rendering from.
		 * @param[in]	ao				Input texture containing ambient occlusion data to be blurred.
		 * @param[in]	sceneDepth		Input texture containing scene depth.
		 * @param[in]	destination		Output texture to which to write the blurred data to.
		 * @param[in]	depthRange		Valid depth range (in view space) within which nearby samples will be averaged.
         * @param[in]	horitonal		Horizontal blur or not
		 */
		void Execute(const RendererView& view, const SPtr<Texture>& ao, const SPtr<Texture>& sceneDepth,
			const SPtr<RenderTexture>& destination, float depthRange, bool horizontal);

    private:
        SPtr<GpuParamBlockBuffer> _paramBuffer;
    };
}
