#include "TeMaterialsPreviewMat.h"

#include "Image/TeTexture.h"
#include "Renderer/TeCamera.h"
#include "Resources/TeResourceManager.h"
#include "Importer/TeTextureImportOptions.h"
#include "Utility/TeTime.h"

namespace te
{
    PreviewMat::PreviewMat()
    {
        _perFrameParamBuffer = _perFrameParamDef.CreateBuffer();
        _perCameraParamBuffer = _perCameraParamDef.CreateBuffer();
        _perObjectParamBuffer = _perObjectParamDef.CreateBuffer();
        _perMaterialParamBuffer = _perMaterialParamDef.CreateBuffer();
        _perLightsParamBuffer = _perLightsParamDef.CreateBuffer();
        _perInstanceParamBuffer = _perInstanceParamDef.CreateBuffer();
    }

    void PreviewMat::BindFrame()
    {
        float time = gTime().GetTime();
        float delta = gTime().GetFrameDelta();

        _perFrameParamDef.gTime.Set(_perFrameParamBuffer, time);
        _perFrameParamDef.gFrameDelta.Set(_perFrameParamBuffer, delta);
        _perFrameParamDef.gUseSkyboxMap.Set(_perFrameParamBuffer, 0);
        _perFrameParamDef.gUseSkyboxIrradianceMap.Set(_perFrameParamBuffer, 0);
        _perFrameParamDef.gSkyboxBrightness.Set(_perFrameParamBuffer, 0.3f);
        _perFrameParamDef.gSceneLightColor.Set(_perFrameParamBuffer, Color::White.GetAsVector4());
    }

    void PreviewMat::BindLight()
    {
        // TODO
    }

    void PreviewMat::BindObject()
    {
        // TODO
    }

    void PreviewMat::BindCamera(const SPtr<Camera>& camera)
    {
        // TODO
    }

    void PreviewMat::BindMaterial(const WPtr<Material>& material)
    {
        // TODO
    }

    PreviewOpaqueMat::PreviewOpaqueMat()
        : PreviewMat()
    { 
        _params->SetParamBlockBuffer("PerFrameBuffer", _perFrameParamBuffer);
        _params->SetParamBlockBuffer("PerCameraBuffer", _perCameraParamBuffer);
        _params->SetParamBlockBuffer("PerObjectBuffer", _perObjectParamBuffer);
        _params->SetParamBlockBuffer("PerMaterialBuffer", _perMaterialParamBuffer);
        _params->SetParamBlockBuffer("PerLightsBuffer", _perLightsParamBuffer);
        _params->SetParamBlockBuffer("PerInstanceBuffer", _perInstanceParamBuffer);

        _params->SetSamplerState("TextureSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Trilinear));
    }

    PreviewTransparentMat::PreviewTransparentMat()
        : PreviewMat()
    { 
        _params->SetParamBlockBuffer("PerFrameBuffer", _perFrameParamBuffer);
        _params->SetParamBlockBuffer("PerCameraBuffer", _perCameraParamBuffer);
        _params->SetParamBlockBuffer("PerObjectBuffer", _perObjectParamBuffer);
        _params->SetParamBlockBuffer("PerMaterialBuffer", _perMaterialParamBuffer);
        _params->SetParamBlockBuffer("PerLightsBuffer", _perLightsParamBuffer);
        _params->SetParamBlockBuffer("PerInstanceBuffer", _perInstanceParamBuffer);

        _params->SetSamplerState("TextureSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Trilinear));
    }
}
