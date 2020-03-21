#include "TeBloomMat.h"
#include "TeRendererUtility.h"

namespace te
{
    BloomParamDef gBloomParamDef;

    BloomMat::BloomMat()
    {
        _paramBuffer = gBloomParamDef.CreateBuffer();
        _params->SetParamBlockBuffer("PerFrameBuffer", _paramBuffer);
        _params->SetSamplerState("BilinearSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Bilinear));
    }

    void BloomMat::Execute(const SPtr<Texture>& source, const SPtr<RenderTarget>& destination, const SPtr<Texture>& emissive, 
        const Color& tint, const float& intensity, UINT32 MSSACount)
    {
        gBloomParamDef.gMSAACount.Set(_paramBuffer, MSSACount, 0);
        gBloomParamDef.gTint.Set(_paramBuffer, tint.GetAsVector4(), 0);
        gBloomParamDef.gIntensity.Set(_paramBuffer, intensity, 0);

        if (MSSACount > 1)
        {
            _params->SetTexture("SourceMapMS", source);
            _params->SetTexture("EmissiveMapMS", emissive);
        }
        else
        {
            _params->SetTexture("SourceMap", source);
            _params->SetTexture("EmissiveMap", emissive);
        }

        RenderAPI& rapi = RenderAPI::Instance();
        rapi.SetRenderTarget(destination);

        Bind();
        gRendererUtility().DrawScreenQuad();
    }
}
