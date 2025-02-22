#include "TeTextureDownsampleMat.h"
#include "Image/TeTexture.h"
#include "Renderer/TeRendererUtility.h"
#include "RenderAPI/TeGpuBuffer.h"

namespace te
{
    TextureDownsampleMat::TextureDownsampleMat()
        : RendererMaterial()
    { }

    void TextureDownsampleMat::Initialize()
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
        {
            _params->SetSamplerState(GPT_PIXEL_PROGRAM, "Sampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::BilinearClamped));
        }
    }

    void TextureDownsampleMat::Execute(const SPtr<Texture>& source, UINT32 mip, const SPtr<RenderTarget>& target)
    {
        _params->SetTexture("SourceMap", source);

        RenderAPI& rapi = RenderAPI::Instance();
        rapi.SetRenderTarget(target);

        Bind();
        
        if (_variation.GetInt("MSAA_COUNT") > 1)
        {
            const TextureProperties& rtProps = source->GetProperties();
            gRendererUtility().DrawScreenQuad(Rect2(0.0f, 0.0f, static_cast<float>(rtProps.GetWidth()), static_cast<float>(rtProps.GetHeight())));
        }
        else
        {
            gRendererUtility().DrawScreenQuad();
        }

        rapi.SetRenderTarget(nullptr);
    }

    TextureDownsampleMat* TextureDownsampleMat::GetVariation(UINT32 msaaCount)
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
