#include "TeFXAAMat.h"
#include "Image/TeTexture.h"
#include "Renderer/TeRendererUtility.h"

namespace te
{
    FXAAParamDef gFXAAParamDef;

    FXAAMat::FXAAMat()
    {
        _paramBuffer = gFXAAParamDef.CreateBuffer();
    }

    void FXAAMat::Initialize()
    {
        _params->SetParamBlockBuffer(GPT_PIXEL_PROGRAM, "PerFrameBuffer", _paramBuffer);
        _params->SetSamplerState(GPT_PIXEL_PROGRAM, "BilinearSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::BilinearClamped));
    }

    void FXAAMat::Execute(const SPtr<Texture>& source, const SPtr<RenderTarget>& destination)
    {
        const TextureProperties& srcProps = source->GetProperties();

        Vector2 invTexSize(1.0f / srcProps.GetWidth(), 1.0f / srcProps.GetHeight());
        gFXAAParamDef.gInvTexSize.Set(_paramBuffer, invTexSize);

        _params->SetTexture(GPT_PIXEL_PROGRAM, "SourceMap", source);

        RenderAPI& rapi = RenderAPI::Instance();
        rapi.SetRenderTarget(destination);

        Bind();
        gRendererUtility().DrawScreenQuad();
    }
}
