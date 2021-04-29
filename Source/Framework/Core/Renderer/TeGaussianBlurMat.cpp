#include "TeGaussianBlurMat.h"
#include "TeRendererUtility.h"
#include "Renderer/TeGpuResourcePool.h"

namespace te
{
    GaussianBlurParamDef gGaussianBlurParamDef;

    GaussianBlurMat::GaussianBlurMat()
    {
        _paramBuffer = gGaussianBlurParamDef.CreateBuffer();
        _params->SetParamBlockBuffer("PerFrameBuffer", _paramBuffer);
        _params->SetSamplerState("BilinearSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Bilinear));
    }

    void GaussianBlurMat::Execute(const SPtr<Texture>& source, const SPtr<RenderTexture>& destination, UINT32 numSamples, UINT32 MSAACount)
    {
        const TextureProperties& srcProps = source->GetProperties();
        const RenderTextureProperties& dstProps = static_cast<const RenderTextureProperties&>(destination->GetProperties());

        POOLED_RENDER_TEXTURE_DESC tempTextureDesc = POOLED_RENDER_TEXTURE_DESC::Create2D(srcProps.GetFormat(),
            dstProps.Width, dstProps.Height, TU_RENDERTARGET, MSAACount);
        SPtr<PooledRenderTexture> tempTexture = gGpuResourcePool().Get(tempTextureDesc);

        gGaussianBlurParamDef.gSourceDimensions.Set(_paramBuffer, Vector2((float)dstProps.Width, (float)dstProps.Height), 0);
        gGaussianBlurParamDef.gMSAACount.Set(_paramBuffer, MSAACount, 0);
        gGaussianBlurParamDef.gNumSamples.Set(_paramBuffer, numSamples, 0);

        // Horizontal pass
        DoPass(true, source, tempTexture->RenderTex, MSAACount);
        // Vertical pass
        DoPass(false, tempTexture->Tex, destination, MSAACount);
    }

    void GaussianBlurMat::DoPass(bool horizontal, const SPtr<Texture>& source, const SPtr<RenderTexture>& destination, UINT32 MSAACount)
    {
        if (MSAACount > 1) _params->SetTexture("SourceMapMS", source);
        else _params->SetTexture("SourceMap", source);

        gGaussianBlurParamDef.gHorizontal.Set(_paramBuffer, horizontal ? 1 : 0, 0);

        RenderAPI& rapi = RenderAPI::Instance();
        rapi.SetRenderTarget(destination);

        Bind();
        gRendererUtility().DrawScreenQuad();
    }
}
