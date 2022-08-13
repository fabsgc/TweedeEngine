#include "TeSSAODownsampleMat.h"
#include "Renderer/TeRendererUtility.h"

namespace te
{
    SSAODownsampleParamDef gSSAODownsampleParamDef;

    SSAODownsampleMat::SSAODownsampleMat()
    { 
        _paramBuffer = gSSAODownsampleParamDef.CreateBuffer();
    }

    void SSAODownsampleMat::Initialize()
    {
        _params->SetParamBlockBuffer(GPT_PIXEL_PROGRAM, "PerFrameBuffer", _paramBuffer);
        _params->SetSamplerState(GPT_PIXEL_PROGRAM, "BilinearSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::BilinearClamped));
    }

    void SSAODownsampleMat::Bind()
    {
        RendererMaterial::Bind();
    }
}
