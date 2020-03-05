#include "TeFXAAMat.h"
#include "TeRendererUtility.h"

namespace te
{
    FXAAParamDef gFXAAParamDef;

    FXAAMat::FXAAMat()
    {
        _paramBuffer = gFXAAParamDef.CreateBuffer();
        _params->SetParamBlockBuffer("PerFrameBuffer", _paramBuffer);
        _params->SetSamplerState("AnisotropicSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Anisotropic));
    }

    void FXAAMat::Execute(const SPtr<Texture>& source, const SPtr<RenderTarget>& destination)
    {
        const TextureProperties& srcProps = source->GetProperties();

        Vector2 invTexSize(1.0f / srcProps.GetWidth(), 1.0f / srcProps.GetHeight());
        gFXAAParamDef.gInvTexSize.Set(_paramBuffer, invTexSize);

        _params->SetTexture("SourceMap", source);

        RenderAPI& rapi = RenderAPI::Instance();
        rapi.SetRenderTarget(destination);

        Bind();
        gRendererUtility().DrawScreenQuad();
    }
}
