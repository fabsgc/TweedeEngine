#include "TeSkyboxMat.h"
#include "TeRendererUtility.h"

namespace te
{
    SkyboxParamDef gSkyboxParamDef;

    SkyboxMat::SkyboxMat()
    { 
        _paramBuffer = gSkyboxParamDef.CreateBuffer();
        _params->SetParamBlockBuffer("PerFrameBuffer", _paramBuffer);
        _params->SetSamplerState("AnisotropicSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Anisotropic));
    }

    void SkyboxMat::Bind(const SPtr<GpuParamBlockBuffer>& perCamera, const SPtr<Texture>& texture, const Color& solidColor)
    {
        _params->SetParamBlockBuffer("PerCameraBuffer", perCamera);
        _params->SetTexture("TextureMap", texture);

        gSkyboxParamDef.gClearColor.Set(_paramBuffer, solidColor);
        gSkyboxParamDef.gUseTexture.Set(_paramBuffer, (texture != nullptr) ? 1 : 0);

        RendererMaterial::Bind();
    }
}
