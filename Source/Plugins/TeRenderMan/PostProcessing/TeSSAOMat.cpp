#include "TeSSAOMat.h"

#include "Renderer/TeRendererUtility.h"

namespace te
{
    SSAOParamDef gSSAOParamDef;

    SSAOMat::SSAOMat()
    { 
        _paramBuffer = gSSAOParamDef.CreateBuffer();
    }

    void SSAOMat::Initialize()
    {
        //_params->SetParamBlockBuffer(GPT_PIXEL_PROGRAM, "PerFrameBuffer", _paramBuffer);
        //_params->SetSamplerState(GPT_PIXEL_PROGRAM, "BilinearClampedSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::BilinearClamped));
        //_params->SetSamplerState(GPT_PIXEL_PROGRAM, "BilinearSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Bilinear));
    }

    void SSAOMat::Execute(const RendererView& view, const SSAOTextureInputs& textures, const SPtr<RenderTexture>& destination, 
        const AmbientOcclusionSettings& settings, bool upSample, bool finalPass, UINT32 quality)
	{
        RenderAPI& rapi = RenderAPI::Instance();
        rapi.SetRenderTarget(destination);

        // TODO

        Bind();
        gRendererUtility().DrawScreenQuad();
    }
}
