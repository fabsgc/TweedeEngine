#include "TeGaussianBlurMat.h"
#include "TeRendererUtility.h"

namespace te
{
    GaussianBlurParamDef gGaussianBlurParamDef;

    GaussianBlurMat::GaussianBlurMat()
    {
        _paramBuffer = gGaussianBlurParamDef.CreateBuffer();
        _params->SetParamBlockBuffer("PerFrameBuffer", _paramBuffer);
        _params->SetSamplerState("BilinearSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Bilinear));
    }

    void GaussianBlurMat::Execute(const SPtr<Texture>& source, const SPtr<RenderTexture>& destination, float filterSize,
        const Color& tint, const SPtr<Texture>& additive, UINT32 MSAACount)
    {
        gGaussianBlurParamDef.gNumSamples.Set(_paramBuffer, STANDARD_MAX_BLUR_SAMPLES, 0);
        gGaussianBlurParamDef.gMSAACount.Set(_paramBuffer, MSAACount, 0);

        if (MSAACount > 1) _params->SetTexture("SourceMapMS", source);
        else _params->SetTexture("SourceMap", source);

        RenderAPI& rapi = RenderAPI::Instance();
        rapi.SetRenderTarget(destination);

        Bind();
        gRendererUtility().DrawScreenQuad();
    }

    UINT32 GaussianBlurMat::CalcStdDistribution(float filterRadius, std::array<float, STANDARD_MAX_BLUR_SAMPLES>& weights,
        std::array<float, STANDARD_MAX_BLUR_SAMPLES>& offsets)
    {
        return 0;
    }

    float GaussianBlurMat::CalcKernelRadius(const SPtr<Texture>& source, float scale, Direction filterDir)
    {
        return 1.0f;
    }

    void GaussianBlurMat::PopulateBuffer(const SPtr<GpuParamBlockBuffer>& buffer, Direction direction,
        const SPtr<Texture>& source, float filterSize, const Color& tint)
    { }
}
