#include "TeSSAOBlurMat.h"

#include "Renderer/TeRendererUtility.h"

namespace te
{
    SSAOBlurParamDef gSSAOBlurParamDef;

    SSAOBlurMat::SSAOBlurMat()
    { 
        _paramBuffer = gSSAOBlurParamDef.CreateBuffer();
    }

    void SSAOBlurMat::Initialize()
    {
        _params->SetParamBlockBuffer(GPT_PIXEL_PROGRAM, "PerFrameBuffer", _paramBuffer);
        _params->SetSamplerState(GPT_PIXEL_PROGRAM, "Sampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::NearestPointClamped));
    }

    void SSAOBlurMat::Execute(const RendererView& view, const SPtr<Texture>& ao, const SPtr<Texture>& sceneDepth,
        const SPtr<RenderTexture>& destination, float depthRange, bool horizontal)
    {
        const RendererViewProperties& viewProps = view.GetProperties();
        const TextureProperties& texProps = ao->GetProperties();

        Vector2 pixelSize;
        pixelSize.x = 1.0f / texProps.GetWidth();
        pixelSize.y = 1.0f / texProps.GetHeight();

        Vector2 pixelOffset(TeZero);
        if (horizontal)
            pixelOffset.x = pixelSize.x;
        else
            pixelOffset.y = pixelSize.y;

        float scale = viewProps.Target.ViewRect.width / (float)texProps.GetWidth();

        gSSAOBlurParamDef.gPixelSize.Set(_paramBuffer, pixelSize);
        gSSAOBlurParamDef.gPixelOffset.Set(_paramBuffer, pixelOffset);
        gSSAOBlurParamDef.gInvDepthThreshold.Set(_paramBuffer, (1.0f / depthRange) / scale);

        _params->SetTexture(GPT_PIXEL_PROGRAM, "InputMap", ao);
        _params->SetTexture(GPT_PIXEL_PROGRAM, "DepthMap", sceneDepth);

        SPtr<GpuParamBlockBuffer> perView = view.GetPerViewBuffer();
        _params->SetParamBlockBuffer("PerCameraBuffer", perView);

        RenderAPI& rapi = RenderAPI::Instance();
        rapi.SetRenderTarget(destination);

        Bind();
        gRendererUtility().DrawScreenQuad();
    }
}
