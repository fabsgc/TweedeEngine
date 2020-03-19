#include "TeMotionBlurMat.h"
#include "TeRendererUtility.h"

namespace te
{
    MotionBlurParamDef gMotionBlurParamDef;

    MotionBlurMat::MotionBlurMat()
    {
        _paramBuffer = gMotionBlurParamDef.CreateBuffer();
        _params->SetParamBlockBuffer("PerFrameBuffer", _paramBuffer);
        _params->SetSamplerState("BilinearSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Bilinear));
    }

    void MotionBlurMat::Execute(const SPtr<Texture>& source, const SPtr<RenderTarget>& destination, const SPtr<Texture>& depth, 
        SPtr<GpuParamBlockBuffer> perViewBuffer, const MotionBlurSettings& settings, INT32 MSSACount)
    {
        UINT32 numSamples;
        switch(settings.Quality)
        {
            default:
            case MotionBlurQuality::VeryLow: numSamples = 4; break;
            case MotionBlurQuality::Low: numSamples = 6; break;
            case MotionBlurQuality::Medium: numSamples = 8; break;
            case MotionBlurQuality::High: numSamples = 12;  break;
            case MotionBlurQuality::Ultra: numSamples = 16;  break;
        }

        gMotionBlurParamDef.gHalfNumSamples.Set(_paramBuffer, numSamples / 2, 0);
        gMotionBlurParamDef.gMSAACount.Set(_paramBuffer, MSSACount, 0);

        if (MSSACount > 1)
        {
            _params->SetTexture("SourceMapMS", source);
            _params->SetTexture("DepthMapMS", depth);
        }
        else
        {
            _params->SetTexture("SourceMap", source);
            _params->SetTexture("DepthMap", depth);
        }

        _params->SetParamBlockBuffer("PerCameraBuffer", perViewBuffer);

        RenderAPI& rapi = RenderAPI::Instance();
        rapi.SetRenderTarget(destination);

        Bind();
        gRendererUtility().DrawScreenQuad();
    }
}
