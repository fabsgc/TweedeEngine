#include "TeMotionBlurMat.h"
#include "Renderer/TeRendererUtility.h"
#include "Utility/TeTime.h"

namespace te
{
    MotionBlurParamDef gMotionBlurParamDef;

    MotionBlurMat::MotionBlurMat()
    {
        _paramBuffer = gMotionBlurParamDef.CreateBuffer();
    }

    void MotionBlurMat::Initialize()
    {
        _params->SetParamBlockBuffer(GPT_PIXEL_PROGRAM, "PerFrameBuffer", _paramBuffer);
        _params->SetSamplerState(GPT_PIXEL_PROGRAM, "Sampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::NearestPointClamped));
    }

    void MotionBlurMat::Execute(const SPtr<Texture>& source, const SPtr<RenderTarget>& destination, const SPtr<Texture>& depth, const SPtr<Texture>& velocity,
        SPtr<GpuParamBlockBuffer> perViewBuffer, const MotionBlurSettings& settings, INT32 MSAACount)
    {
        UINT32 numSamples;
        switch(settings.Quality)
        {
            default:
            case MotionBlurQuality::Low: numSamples = 8; break;
            case MotionBlurQuality::Medium: numSamples = 12; break;
            case MotionBlurQuality::High: numSamples = 16;  break;
            case MotionBlurQuality::Ultra: numSamples = 24;  break;
        }

        gMotionBlurParamDef.gFrameDelta.Set(_paramBuffer, gTime().GetFrameDelta(), 0);
        gMotionBlurParamDef.gHalfNumSamples.Set(_paramBuffer, numSamples / 2, 0);
        gMotionBlurParamDef.gBlurType.Set(_paramBuffer, (INT32)settings.Domain, 0);
        gMotionBlurParamDef.gMSAACount.Set(_paramBuffer, MSAACount, 0);

        if (MSAACount > 1)
        {
            _params->SetTexture(GPT_PIXEL_PROGRAM, "SourceMapMS", source);
            _params->SetTexture("DepthMapMS", depth);
            _params->SetTexture(GPT_PIXEL_PROGRAM, "VelocityMapMS", velocity);
        }
        else
        {
            _params->SetTexture(GPT_PIXEL_PROGRAM, "SourceMap", source);
            _params->SetTexture("DepthMap", depth);
            _params->SetTexture(GPT_PIXEL_PROGRAM, "VelocityMap", velocity);
        }

        _params->SetParamBlockBuffer("PerCameraBuffer", perViewBuffer);

        RenderAPI& rapi = RenderAPI::Instance();
        rapi.SetRenderTarget(destination);

        Bind();
        gRendererUtility().DrawScreenQuad();
    }
}
