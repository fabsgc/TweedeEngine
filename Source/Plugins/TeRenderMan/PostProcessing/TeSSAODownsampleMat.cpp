#include "TeSSAODownsampleMat.h"

#include "Renderer/TeRendererUtility.h"

namespace te
{
    SSAODownsampleParamDef gSSAODownsampleParamDef;

    SSAODownsampleMat::SSAODownsampleMat()
    { 
        _paramBuffer = gSSAODownsampleParamDef.CreateBuffer();
    }

    void SSAODownsampleMat::Initialize()
    {
        _params->SetParamBlockBuffer(GPT_PIXEL_PROGRAM, "PerFrameBuffer", _paramBuffer);
        _params->SetSamplerState(GPT_PIXEL_PROGRAM, "BilinearSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::BilinearClamped));
    }

    void SSAODownsampleMat::Execute(const RendererView& view, const SPtr<Texture>& sceneDepth, const SPtr<Texture>& sceneNormals,
        const SPtr<RenderTexture>& destination, float depthRange)
    {
        _params->SetTexture(GPT_PIXEL_PROGRAM, "DepthMap", sceneDepth);
        _params->SetTexture(GPT_PIXEL_PROGRAM, "NormalsMap", sceneNormals);

        const RendererViewProperties& viewProps = view.GetProperties();
        const RenderTargetProperties& rtProps = destination->GetProperties();

        Vector2 pixelSize;
        pixelSize.x = 1.0f / rtProps.Width;
        pixelSize.y = 1.0f / rtProps.Height;

        float scale = viewProps.Target.ViewRect.width / (float)rtProps.Width;

        gSSAODownsampleParamDef.gPixelSize.Set(_paramBuffer, pixelSize);
        gSSAODownsampleParamDef.gInvDepthThreshold.Set(_paramBuffer, (1.0f / depthRange) / scale);

        SPtr<GpuParamBlockBuffer> perView = view.GetPerViewBuffer();
        _params->SetParamBlockBuffer("PerCameraBuffer", perView);

        RenderAPI& rapi = RenderAPI::Instance();
        rapi.SetRenderTarget(destination);

        Bind();
        gRendererUtility().DrawScreenQuad();
    }
}
