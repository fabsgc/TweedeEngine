#include "TeMaterialsPreviewMat.h"

#include "Image/TeTexture.h"
#include "Renderer/TeCamera.h"
#include "Material/TeMaterial.h"
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
        _perFrameParamDef.gSceneLightColor.Set(_perFrameParamBuffer, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    void PreviewMat::BindLight()
    {
        static MaterialsPreviewMat::LightData lightData;

        lightData.Position = Vector3(0.0f, 3.0f, 3.0f);
        lightData.Direction = Vector3(0.75f, -0.5f, -0.5f);
        lightData.Intensity = 0.4f;
        lightData.AttenuationRadius = 1.0f;
        lightData.LinearAttenuation = 0.08f;
        lightData.QuadraticAttenuation = 0.0f;
        lightData.BoundsRadius = 512.0f;
        lightData.Color = Vector3::ONE;
        lightData.Type = 1.0f;

        _perLightsParamDef.gLights.Set(_perLightsParamBuffer, lightData, 0);
        _perLightsParamDef.gLightsNumber.Set(_perLightsParamBuffer, 1);
    }

    void PreviewMat::BindObject()
    {
        static const Matrix4& tfrm = Matrix4::IDENTITY;
        static const Matrix4& tfrmNoScale = Matrix4::IDENTITY;
        static const UINT32 layer = 0;

        _perObjectParamDef.gMatWorld.Set(_perObjectParamBuffer, tfrmNoScale);
        _perObjectParamDef.gMatInvWorld.Set(_perObjectParamBuffer, tfrm.InverseAffine());
        _perObjectParamDef.gMatWorldNoScale.Set(_perObjectParamBuffer, tfrmNoScale);
        _perObjectParamDef.gMatInvWorldNoScale.Set(_perObjectParamBuffer, tfrmNoScale.InverseAffine());
        _perObjectParamDef.gMatPrevWorld.Set(_perObjectParamBuffer, tfrm);
        _perObjectParamDef.gLayer.Set(_perObjectParamBuffer, (INT32)layer);
        _perObjectParamDef.gHasAnimation.Set(_perObjectParamBuffer, 0);
        _perObjectParamDef.gWriteVelocity.Set(_perObjectParamBuffer, 0);
        _perObjectParamDef.gCastLights.Set(_perObjectParamBuffer, 1);
    }

    void PreviewMat::BindCamera(const SPtr<Camera>& camera)
    {
        static const Transform tfrm = camera->GetTransform();
        static const Vector3 ViewOrigin = tfrm.GetPosition();
        static const Vector3 ViewDirection = tfrm.GetForward();
        static const Matrix4 ProjTransform = camera->GetProjectionMatrixRS();
        static const Matrix4 ViewTransform = camera->GetViewMatrix();
        static const ProjectionType ProjType = camera->GetProjectionType();
        static const Matrix4 viewProj = ProjTransform * ViewTransform;

        _perCameraParamDef.gMatProj.Set(_perCameraParamBuffer, ProjTransform);
        _perCameraParamDef.gMatView.Set(_perCameraParamBuffer, ViewTransform);
        _perCameraParamDef.gMatViewProj.Set(_perCameraParamBuffer, viewProj);
        _perCameraParamDef.gMatPrevViewProj.Set(_perCameraParamBuffer, viewProj);
        _perCameraParamDef.gViewDir.Set(_perCameraParamBuffer, ViewDirection);
        _perCameraParamDef.gViewOrigin.Set(_perCameraParamBuffer, ViewOrigin);
        _perCameraParamDef.gViewportX.Set(_perCameraParamBuffer, static_cast<UINT32>(camera->GetViewport()->GetArea().x));
        _perCameraParamDef.gViewportY.Set(_perCameraParamBuffer, static_cast<UINT32>(camera->GetViewport()->GetArea().y));

        // TODO Be careful not all parameters are set (because, most of them are useless)
    }

    void PreviewMat::BindMaterial(const WPtr<Material>& material)
    {
        const MaterialProperties& properties = material.lock()->GetProperties();

        _perMaterialParamDef.gAmbient.Set(_perMaterialParamBuffer, properties.Ambient.GetAsVector4());
        _perMaterialParamDef.gDiffuse.Set(_perMaterialParamBuffer, properties.Diffuse.GetAsVector4());
        _perMaterialParamDef.gEmissive.Set(_perMaterialParamBuffer, properties.Emissive.GetAsVector4());
        _perMaterialParamDef.gSpecular.Set(_perMaterialParamBuffer, properties.Specular.GetAsVector4());
        _perMaterialParamDef.gTextureRepeat.Set(_perMaterialParamBuffer, properties.TextureRepeat);
        _perMaterialParamDef.gTextureOffset.Set(_perMaterialParamBuffer, properties.TextureOffset);
        _perMaterialParamDef.gUseDiffuseMap.Set(_perMaterialParamBuffer, properties.UseDiffuseMap ? 1 : 0);
        _perMaterialParamDef.gUseEmissiveMap.Set(_perMaterialParamBuffer, properties.UseEmissiveMap ? 1 : 0);
        _perMaterialParamDef.gUseNormalMap.Set(_perMaterialParamBuffer, properties.UseNormalMap ? 1 : 0);
        _perMaterialParamDef.gUseSpecularMap.Set(_perMaterialParamBuffer, properties.UseSpecularMap ? 1 : 0);
        _perMaterialParamDef.gUseBumpMap.Set(_perMaterialParamBuffer, properties.UseBumpMap ? 1 : 0);
        _perMaterialParamDef.gUseParallaxMap.Set(_perMaterialParamBuffer, properties.UseParallaxMap ? 1 : 0);
        _perMaterialParamDef.gUseTransparencyMap.Set(_perMaterialParamBuffer, properties.UseTransparencyMap ? 1 : 0);
        _perMaterialParamDef.gUseReflectionMap.Set(_perMaterialParamBuffer, properties.UseReflectionMap ? 1 : 0);
        _perMaterialParamDef.gUseOcclusionMap.Set(_perMaterialParamBuffer, properties.UseOcclusionMap ? 1 : 0);
        _perMaterialParamDef.gUseEnvironmentMap.Set(_perMaterialParamBuffer, properties.UseEnvironmentMap ? 1 : 0);
        _perMaterialParamDef.gUseIrradianceMap.Set(_perMaterialParamBuffer, properties.UseIrradianceMap ? 1 : 0);
        _perMaterialParamDef.gUseGlobalIllumination.Set(_perMaterialParamBuffer, properties.UseGlobalIllumination ? 1 : 0);
        _perMaterialParamDef.gSpecularPower.Set(_perMaterialParamBuffer, properties.SpecularPower);
        _perMaterialParamDef.gSpecularStrength.Set(_perMaterialParamBuffer, properties.SpecularStrength);
        _perMaterialParamDef.gTransparency.Set(_perMaterialParamBuffer, properties.Transparency);
        _perMaterialParamDef.gIndexOfRefraction.Set(_perMaterialParamBuffer, properties.IndexOfRefraction);
        _perMaterialParamDef.gRefraction.Set(_perMaterialParamBuffer, properties.Refraction);
        _perMaterialParamDef.gReflection.Set(_perMaterialParamBuffer, properties.Reflection);
        _perMaterialParamDef.gBumpScale.Set(_perMaterialParamBuffer, properties.BumpScale);
        _perMaterialParamDef.gParallaxScale.Set(_perMaterialParamBuffer, properties.ParallaxScale);
        _perMaterialParamDef.gAlphaThreshold.Set(_perMaterialParamBuffer, properties.AlphaThreshold);
        _perMaterialParamDef.gParallaxSamples.Set(_perMaterialParamBuffer, properties.ParallaxSamples);
    }

    void PreviewMat::BindTextures(const WPtr<Material>& material, SPtr<GpuParams> params)
    {
        const MaterialProperties& properties = material.lock()->GetProperties();

        if (properties.UseDiffuseMap)
            params->SetTexture("DiffuseMap", material.lock()->GetTexture("DiffuseMap"));
        if(properties.UseEmissiveMap)
            params->SetTexture("EmissiveMap", material.lock()->GetTexture("EmissiveMap"));
        if(properties.UseNormalMap)
            params->SetTexture("NormalMap", material.lock()->GetTexture("NormalMap"));
        if (properties.UseSpecularMap)
            params->SetTexture("SpecularMap", material.lock()->GetTexture("SpecularMap"));
        if (properties.UseBumpMap)
            params->SetTexture("BumpMap", material.lock()->GetTexture("BumpMap"));
        if (properties.UseParallaxMap)
            params->SetTexture("ParallaxMap", material.lock()->GetTexture("ParallaxMap"));
        if (properties.UseTransparencyMap)
            params->SetTexture("TransparencyMap", material.lock()->GetTexture("TransparencyMap"));
        if (properties.UseReflectionMap)
            params->SetTexture("ReflectionMap", material.lock()->GetTexture("ReflectionMap"));
        if (properties.UseOcclusionMap)
            params->SetTexture("OcclusionMap", material.lock()->GetTexture("OcclusionMap"));
        if (properties.UseEnvironmentMap)
            params->SetTexture("EnvironmentMap", material.lock()->GetTexture("EnvironmentMap"));
        if (properties.UseIrradianceMap)
            params->SetTexture("IrradianceMap", material.lock()->GetTexture("IrradianceMap"));
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

        _params->SetSamplerState("TextureSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Anisotropic));
    }

    void PreviewOpaqueMat::BindTextures(const WPtr<Material>& material)
    {
        PreviewMat::BindTextures(material, _params);
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

        _params->SetSamplerState("TextureSampler", gBuiltinResources().GetBuiltinSampler(BuiltinSampler::Anisotropic));
    }

    void PreviewTransparentMat::BindTextures(const WPtr<Material>& material)
    {
        PreviewMat::BindTextures(material, _params);
    }
}
