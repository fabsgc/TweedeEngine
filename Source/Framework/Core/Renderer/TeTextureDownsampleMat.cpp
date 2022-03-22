#include "TeTextureDownsampleMat.h"
#include "Image/TeTexture.h"
#include "Renderer/TeRendererUtility.h"
#include "RenderAPI/TeGpuBuffer.h"

namespace te
{
    TextureDownsampleParamDef gTextureDownsampleParamDef;

    TextureDownsampleMat::TextureDownsampleMat()
    {
        _paramBuffer = gTextureDownsampleParamDef.CreateBuffer();
        _params->SetParamBlockBuffer("PerFrameBuffer", _paramBuffer);
        _params->SetSamplerState("BilinearSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Bilinear));
    }

    void TextureDownsampleMat::Execute(const SPtr<Texture>& source, UINT32 mip, const SPtr<RenderTarget>& target)
    {
        _params->SetTexture("SourceMap", source);
        gTextureDownsampleParamDef.gMipLevel.Set(_paramBuffer, mip);

        RenderAPI& rapi = RenderAPI::Instance();
        rapi.SetRenderTarget(target);

        Bind();
        gRendererUtility().DrawScreenQuad();
    }
}
