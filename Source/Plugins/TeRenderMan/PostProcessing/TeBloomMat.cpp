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

        const auto& variationParams = _variation.GetParams();
        const bool isMultiSampledVariation = std::find_if(variationParams.begin(), variationParams.end(),
            [](const Pair<String, ShaderVariation::Param>& x) { 
                if (x.second.Name == "MSAA_COUNT") {
                    if (x.second.Type == ShaderVariation::ParamType::UInt && x.second.Ui > 1)
                        return true;
                }

                return false;
            }) != variationParams.end();

        if (!isMultiSampledVariation)
            _params->SetSamplerState(GPT_PIXEL_PROGRAM, "Sampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::NearestPointClamped));
    }

    void BloomMat::Execute(const SPtr<Texture>& source, const SPtr<RenderTarget>& destination, const SPtr<Texture>& emissive, 
        const float& intensity)
    {
        gBloomParamDef.gIntensity.Set(_paramBuffer, intensity, 0);
        _params->SetTexture(GPT_PIXEL_PROGRAM, "SourceMap", source);
        _params->SetTexture(GPT_PIXEL_PROGRAM, "EmissiveMap", emissive);

        RenderAPI& rapi = RenderAPI::Instance();

        Bind();
        rapi.SetRenderTarget(destination);
        gRendererUtility().DrawScreenQuad();
        rapi.SetRenderTarget(nullptr);
    }

    BloomMat* BloomMat::GetVariation(UINT32 msaaCount)
    {
        switch (msaaCount)
        {
        case 1:
            return Get(GetVariation<1>());
        case 2:
            return Get(GetVariation<2>());
        case 4:
            return Get(GetVariation<4>());
        default:
        case 8:
            return Get(GetVariation<8>());
        }
    }
}
