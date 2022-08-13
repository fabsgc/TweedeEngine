#include "TeSSAOBlurMat.h"

#include "Renderer/TeRendererUtility.h"

namespace te
{
    SSAOBlurParamDef gSSAOBlurParamDef;

    SSAOBlurMat::SSAOBlurMat()
    { 
        _paramBuffer = gSSAOBlurParamDef.CreateBuffer();
    }

    void SSAOBlurMat::Initialize()
    {
        //_params->SetParamBlockBuffer(GPT_PIXEL_PROGRAM, "PerFrameBuffer", _paramBuffer);
        //_params->SetSamplerState(GPT_PIXEL_PROGRAM, "BilinearSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::BilinearClamped));
    }

    void SSAOBlurMat::Execute(const RendererView& view, const SPtr<Texture>& ao, const SPtr<Texture>& sceneDepth,
			const SPtr<RenderTexture>& destination, float depthRange, bool horizontal)
    {
        Bind();
        gRendererUtility().DrawScreenQuad();
    }
}
