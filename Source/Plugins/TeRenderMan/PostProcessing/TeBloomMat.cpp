#include "TeBloomMat.h"
#include "Renderer/TeRendererUtility.h"

namespace te
{
    BloomParamDef gBloomParamDef;

    BloomMat::BloomMat()
    {
        _paramBuffer = gBloomParamDef.CreateBuffer();
    }

    void BloomMat::Initialize()
    {
        _params->SetParamBlockBuffer(GPT_PIXEL_PROGRAM, "PerFrameBuffer", _paramBuffer);
        _params->SetSamplerState(GPT_PIXEL_PROGRAM, "NoFilterSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::BilinearClamped));
    }

    void BloomMat::Execute(const SPtr<Texture>& source, const SPtr<RenderTarget>& destination, const SPtr<Texture>& emissive, 
        const float& intensity, UINT32 MSAACount)
    {
        gBloomParamDef.gMSAACount.Set(_paramBuffer, MSAACount, 0);
        gBloomParamDef.gIntensity.Set(_paramBuffer, intensity, 0);

        if (MSAACount > 1)
        {
            _params->SetTexture(GPT_PIXEL_PROGRAM, "SourceMapMS", source);
            _params->SetTexture(GPT_PIXEL_PROGRAM, "EmissiveMapMS", emissive);
        }
        else
        {
            _params->SetTexture(GPT_PIXEL_PROGRAM, "SourceMap", source);
            _params->SetTexture(GPT_PIXEL_PROGRAM, "EmissiveMap", emissive);
        }

        RenderAPI& rapi = RenderAPI::Instance();
        rapi.SetRenderTarget(destination);

        Bind();
        gRendererUtility().DrawScreenQuad();
    }
}
