#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeParamBlocks.h"
#include "Renderer/TeRendererMaterial.h"

namespace te
{
    namespace MaterialsPreviewMat
    {
        struct PerInstanceData
        {
            Matrix4 gMatWorld;
            Matrix4 gMatInvWorld;
            Matrix4 gMatWorldNoScale;
            Matrix4 gMatInvWorldNoScale;
            Matrix4 gMatPrevWorld;
            UINT32  gLayer;
            UINT32  gHasAnimation;
            UINT32  gWriteVelocity;
            UINT32  gCastLights;
        };

        struct LightData
        {
            Vector3 Color;
            float   Type;
            Vector3 Position;
            float   Intensity;
            Vector3 Direction;
            float   AttenuationRadius;
            Vector3 SpotAngles;
            float   BoundsRadius;
            float   LinearAttenuation;
            float   QuadraticAttenuation;
            Vector2 Padding;
        };

        TE_PARAM_BLOCK_BEGIN(PerCameraParamDef)
            TE_PARAM_BLOCK_ENTRY(Vector3, gViewDir)
            TE_PARAM_BLOCK_ENTRY(UINT32, gViewportX)
            TE_PARAM_BLOCK_ENTRY(Vector3, gViewOrigin)
            TE_PARAM_BLOCK_ENTRY(UINT32, gViewportY)
            TE_PARAM_BLOCK_ENTRY(Matrix4, gMatViewProj)
            TE_PARAM_BLOCK_ENTRY(Matrix4, gMatView)
            TE_PARAM_BLOCK_ENTRY(Matrix4, gMatProj)
            TE_PARAM_BLOCK_ENTRY(Matrix4, gMatPrevViewProj)
            TE_PARAM_BLOCK_ENTRY(Matrix4, gNDCToPrevNDC)
            TE_PARAM_BLOCK_ENTRY(Vector4, gClipToUVScaleOffset)
            TE_PARAM_BLOCK_ENTRY(Vector4, gUVToClipScaleOffset)
        TE_PARAM_BLOCK_END

        TE_PARAM_BLOCK_BEGIN(PerFrameParamDef)
            TE_PARAM_BLOCK_ENTRY(float, gTime)
            TE_PARAM_BLOCK_ENTRY(float, gFrameDelta)
            TE_PARAM_BLOCK_ENTRY(INT32, gUseSkyboxMap)
            TE_PARAM_BLOCK_ENTRY(INT32, gUseSkyboxIrradianceMap)
            TE_PARAM_BLOCK_ENTRY(float, gSkyboxBrightness)
            TE_PARAM_BLOCK_ENTRY(Vector4, gSceneLightColor)
        TE_PARAM_BLOCK_END

        TE_PARAM_BLOCK_BEGIN(PerObjectParamDef)
            TE_PARAM_BLOCK_ENTRY(Matrix4, gMatWorld)
            TE_PARAM_BLOCK_ENTRY(Matrix4, gMatInvWorld)
            TE_PARAM_BLOCK_ENTRY(Matrix4, gMatWorldNoScale)
            TE_PARAM_BLOCK_ENTRY(Matrix4, gMatInvWorldNoScale)
            TE_PARAM_BLOCK_ENTRY(Matrix4, gMatPrevWorld)
            TE_PARAM_BLOCK_ENTRY(INT32, gLayer)
            TE_PARAM_BLOCK_ENTRY(INT32, gHasAnimation)
            TE_PARAM_BLOCK_ENTRY(INT32, gWriteVelocity)
            TE_PARAM_BLOCK_ENTRY(INT32, gCastLights)
        TE_PARAM_BLOCK_END

        TE_PARAM_BLOCK_BEGIN(PerMaterialParamDef)
            TE_PARAM_BLOCK_ENTRY(Vector4, gAmbient)
            TE_PARAM_BLOCK_ENTRY(Vector4, gDiffuse)
            TE_PARAM_BLOCK_ENTRY(Vector4, gEmissive)
            TE_PARAM_BLOCK_ENTRY(Vector4, gSpecular)
            TE_PARAM_BLOCK_ENTRY(Vector2, gTextureRepeat)
            TE_PARAM_BLOCK_ENTRY(Vector2, gTextureOffset)
            TE_PARAM_BLOCK_ENTRY(INT32, gUseDiffuseMap)
            TE_PARAM_BLOCK_ENTRY(INT32, gUseEmissiveMap)
            TE_PARAM_BLOCK_ENTRY(INT32, gUseNormalMap)
            TE_PARAM_BLOCK_ENTRY(INT32, gUseSpecularMap)
            TE_PARAM_BLOCK_ENTRY(INT32, gUseBumpMap)
            TE_PARAM_BLOCK_ENTRY(INT32, gUseParallaxMap)
            TE_PARAM_BLOCK_ENTRY(INT32, gUseTransparencyMap)
            TE_PARAM_BLOCK_ENTRY(INT32, gUseReflectionMap)
            TE_PARAM_BLOCK_ENTRY(INT32, gUseOcclusionMap)
            TE_PARAM_BLOCK_ENTRY(INT32, gUseEnvironmentMap)
            TE_PARAM_BLOCK_ENTRY(INT32, gUseIrradianceMap)
            TE_PARAM_BLOCK_ENTRY(INT32, gUseGlobalIllumination)
            TE_PARAM_BLOCK_ENTRY(float, gSpecularPower)
            TE_PARAM_BLOCK_ENTRY(float, gSpecularStrength)
            TE_PARAM_BLOCK_ENTRY(float, gTransparency)
            TE_PARAM_BLOCK_ENTRY(float, gIndexOfRefraction)
            TE_PARAM_BLOCK_ENTRY(float, gRefraction)
            TE_PARAM_BLOCK_ENTRY(float, gReflection)
            TE_PARAM_BLOCK_ENTRY(float, gBumpScale)
            TE_PARAM_BLOCK_ENTRY(float, gParallaxScale)
            TE_PARAM_BLOCK_ENTRY(float, gAlphaThreshold)
            TE_PARAM_BLOCK_ENTRY(INT32, gParallaxSamples)
        TE_PARAM_BLOCK_END

        TE_PARAM_BLOCK_BEGIN(PerLightsParamDef)
            TE_PARAM_BLOCK_ENTRY_ARRAY(LightData, gLights, 24)
            TE_PARAM_BLOCK_ENTRY(INT32, gLightsNumber)
        TE_PARAM_BLOCK_END

        TE_PARAM_BLOCK_BEGIN(PerInstanceParamDef)
            TE_PARAM_BLOCK_ENTRY_ARRAY(PerInstanceData, gInstances, 1)
        TE_PARAM_BLOCK_END
    }
    
    class PreviewMat
    {
    public:
        PreviewMat();

        /** Set frame gpu buffer statically */
        void BindFrame();

        /** Set light gpu buffer for the given light */
        void BindLight();

        /** Set object gpu buffer for the given renderable */
        void BindObject();

        /** Set camera gpu buffer for the given camera */
        void BindCamera(const SPtr<Camera>& camera);

        /** Set material gpu buffer for the given material */
        void BindMaterial(const WPtr<Material>& material);

    protected:
        MaterialsPreviewMat::PerCameraParamDef _perCameraParamDef;
        MaterialsPreviewMat::PerFrameParamDef _perFrameParamDef;
        MaterialsPreviewMat::PerObjectParamDef _perObjectParamDef;
        MaterialsPreviewMat::PerMaterialParamDef _perMaterialParamDef;
        MaterialsPreviewMat::PerLightsParamDef _perLightsParamDef;
        MaterialsPreviewMat::PerInstanceParamDef _perInstanceParamDef;

        SPtr<GpuParamBlockBuffer> _perCameraParamBuffer;
        SPtr<GpuParamBlockBuffer> _perFrameParamBuffer;
        SPtr<GpuParamBlockBuffer> _perObjectParamBuffer;
        SPtr<GpuParamBlockBuffer> _perMaterialParamBuffer;
        SPtr<GpuParamBlockBuffer> _perLightsParamBuffer;
        SPtr<GpuParamBlockBuffer> _perInstanceParamBuffer;
    };

    /** Shader that performs material's preview for opaque objects. */
    class PreviewOpaqueMat : public PreviewMat, public RendererMaterial<PreviewOpaqueMat>
    {
        RMAT_DEF(BuiltinShader::PreviewOpaque);

    public:
        PreviewOpaqueMat();
    };

    /** Shader that performs material's preview for transparent objects. */
    class PreviewTransparentMat : public PreviewMat, public RendererMaterial<PreviewTransparentMat>
    {
        RMAT_DEF(BuiltinShader::PreviewTransparent);

    public:
        PreviewTransparentMat();
    };
}
