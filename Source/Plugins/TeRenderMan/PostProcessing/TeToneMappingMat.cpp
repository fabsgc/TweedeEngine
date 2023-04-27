#include "TeToneMappingMat.h"
#include "Renderer/TeRendererUtility.h"

namespace te
{
    ToneMappingParamDef gToneMappingParamDef;

    ToneMappingMat::ToneMappingMat()
    {
        _paramBuffer = gToneMappingParamDef.CreateBuffer();
    }

    void ToneMappingMat::Initialize()
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

    void ToneMappingMat::Execute(const SPtr<Texture>& ssao, const SPtr<Texture>& source, const SPtr<RenderTarget>& destination, INT32 MSAACount,
        float gamma, float exposure, float contrast, float brightness, bool gammaOnly)
    {
        gToneMappingParamDef.gGamma.Set(_paramBuffer, gamma, 0);
        gToneMappingParamDef.gExposure.Set(_paramBuffer, exposure, 0);
        gToneMappingParamDef.gContrast.Set(_paramBuffer, contrast, 0);
        gToneMappingParamDef.gBrightness.Set(_paramBuffer, brightness, 0);

        _params->SetTexture(GPT_PIXEL_PROGRAM, "SourceMap", source);
        _params->SetTexture(GPT_PIXEL_PROGRAM, "SSAOMap", ssao);

        RenderAPI& rapi = RenderAPI::Instance();
        rapi.SetRenderTarget(destination);

        Bind();
        gRendererUtility().DrawScreenQuad();
    }

    ToneMappingMat* ToneMappingMat::GetVariation(UINT32 msaaCount, bool gammaOnly)
    {
        if (msaaCount > 1)
        {
            if(gammaOnly)
            {
                switch (msaaCount)
                {
                case 2:
                    return Get(GetVariation<2, true>());
                case 4:
                    return Get(GetVariation<4, true>());
                default:
                case 8:
                    return Get(GetVariation<8, true>());
                }
            }
            else
            {
                switch(msaaCount)
                {
                case 2:
                    return Get(GetVariation<2, false>());
                case 4:
                    return Get(GetVariation<4, false>());
                default:
                case 8:
                    return Get(GetVariation<8, false>());
                }
            }
        }
        else
        {
            if(gammaOnly)
                return Get(GetVariation<1, true>());
            else
                return Get(GetVariation<1, false>());
        }
    }
}
