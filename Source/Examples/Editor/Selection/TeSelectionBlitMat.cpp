#include "TeSelectionBlitMat.h"

#include "Renderer/TeRendererUtility.h"

namespace te
{
    SelectionBlitMat::SelectionBlitMat()
    { }

    void SelectionBlitMat::Initialize()
    {
        _params->SetSamplerState(GPT_PIXEL_PROGRAM, "Sampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::BilinearClamped));
    }

    void SelectionBlitMat::Execute(const SPtr<Texture>& viewport, const SPtr<Texture>& outline, const SPtr<Texture>& blurredOutline)
    {
        _params->SetTexture(GPT_PIXEL_PROGRAM, "ViewportMap", viewport);
        _params->SetTexture(GPT_PIXEL_PROGRAM, "OutlineMap", outline);
        _params->SetTexture(GPT_PIXEL_PROGRAM, "BlurredOutlineMap", blurredOutline);

        Bind();
        gRendererUtility().DrawScreenQuad();
    }
}
