#include "TeBlitMat.h"
#include "Renderer/TeRendererUtility.h"

namespace te
{
    BlitMat::BlitMat()
    { }

    void BlitMat::Initialize()
    {
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

    void BlitMat::Execute(const SPtr<Texture>& source, const Rect2& area, bool flipUV)
    {
        _params->SetTexture(GPT_PIXEL_PROGRAM, "SourceMap", source);

        Bind();
        gRendererUtility().DrawScreenQuad(area, Vector2I(1, 1), 1, flipUV);
    }

    BlitMat* BlitMat::GetVariation(UINT32 msaaCount, bool isColor)
    {
        if (msaaCount > 1)
        {
            if(isColor)
            {
                switch (msaaCount)
                {
                case 2:
                    return Get(GetVariation<2, 0>());
                case 4:
                    return Get(GetVariation<4, 0>());
                default:
                case 8:
                    return Get(GetVariation<8, 0>());
                }
            }
            else
            {
                switch(msaaCount)
                {
                case 2:
                    return Get(GetVariation<2, 1>());
                case 4:
                    return Get(GetVariation<4, 1>());
                default:
                case 8:
                    return Get(GetVariation<8, 1>());
                }
            }
        }
        else
        {
            if(isColor)
                return Get(GetVariation<1, 0>());
            else
                return Get(GetVariation<1, 1>());
        }
    }
}
