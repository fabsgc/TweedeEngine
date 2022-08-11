#include "TeSkyboxMat.h"
#include "Renderer/TeRendererUtility.h"

namespace te
{
    SkyboxParamDef gSkyboxParamDef;

    SkyboxMat::SkyboxMat()
    { 
        _paramBuffer = gSkyboxParamDef.CreateBuffer();
    }

    void SkyboxMat::Initialize()
    {
        _params->SetParamBlockBuffer(GPT_PIXEL_PROGRAM, "PerFrameBuffer", _paramBuffer);
        _params->SetSamplerState(GPT_PIXEL_PROGRAM, "BilinearSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Bilinear));
    }

    void SkyboxMat::Bind(const SPtr<GpuParamBlockBuffer>& perCamera, const SPtr<Texture>& texture, const Color& solidColor, const float& brightness)
    {
        _params->SetParamBlockBuffer("PerCameraBuffer", perCamera);
        _params->SetTexture(GPT_PIXEL_PROGRAM, "TextureMap", texture);

        gSkyboxParamDef.gClearColor.Set(_paramBuffer, solidColor);
        gSkyboxParamDef.gBrightness.Set(_paramBuffer, brightness);
        gSkyboxParamDef.gUseTexture.Set(_paramBuffer, (texture != nullptr) ? 1 : 0);

        RendererMaterial::Bind();
    }
}
