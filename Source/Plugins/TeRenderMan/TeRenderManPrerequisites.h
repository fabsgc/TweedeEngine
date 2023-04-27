#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"
#include "Renderer/TeParamBlocks.h"
#include "Math/TeMatrix4.h"
#include "Math/TeVector2.h"

#define STANDARD_FORWARD_MIN_INSTANCED_BLOCK_SIZE 2
#define STANDARD_FORWARD_MAX_INSTANCED_BLOCK_SIZE 128

#define STANDARD_FORWARD_MAX_INSTANCED_BLOCKS_NUMBER 128

#define STANDARD_FORWARD_MAX_NUM_LIGHTS 24

namespace te
{
    struct PerCameraData
    {
        Vector3 ViewDir;
        float  ViewportX;
        Vector3 ViewOrigin;
        float  ViewportY;
        Matrix4 MatViewProj;
        Matrix4 MatView;
        Matrix4 MatProj;
        Matrix4 MatPrevViewProj;
        Matrix4 NDCToPrevNDC;
        Vector2 DeviceZToWorldZ;
        Vector2 NDCZToWorldZ;
        Vector2 NDCZToDeviceZ;
        Vector2 NearFar;
        Vector4 ClipToUVScaleOffset;
        Vector4 UVToClipScaleOffset;
        UINT32  UseSRGB;
        Vector3 Padding; // # PADDING
    };

    struct PerInstanceData
    {
        Matrix4 MatWorld;
        Matrix4 MatInvWorld;
        Matrix4 MatWorldNoScale;
        Matrix4 MatInvWorldNoScale;
        Matrix4 MatPrevWorld;
        UINT32  Layer;
        UINT32  HasAnimation;
        UINT32  WriteVelocity;
        UINT32  CastLights;
    };

    struct PerMaterialData
    {
        Vector4 BaseColor;
        float   Metallic;
        float   Roughness;
        float   Reflectance;
        float   Occlusion;
        Vector4 Emissive;
        Vector4 SheenColor;
        float   SheenRoughness;
        float   ClearCoat;
        float   ClearCoatRoughness;
        float   SurSurfacePower;
        Vector4 SubsurfaceColor;
        float   Anisotropy;
        Vector3 AnisotropyDirection;
        Vector2 UV0Repeat;
        Vector2 UV0Offset;
        float   ParallaxScale;
        UINT32  ParallaxSamples;
        float   MicroThickness;
        float   Thickness;
        float   Transmission;
        Vector3 Absorption;
        UINT32  RefractionType;
        float   AlphaTreshold;
        Vector2 Padding1;
    };

    struct PerLightData
    {
        Vector3 Color;
        UINT32  Type;
        Vector3 Position;
        float   Intensity;
        Vector3 Direction;
        float   AttenuationRadius;
        Vector3 SpotAngles;
        float   BoundsRadius;
        float   LinearAttenuation;
        float   QuadraticAttenuation;
        UINT32  CastShadows;
        float   Padding2; // # PADDING
    };

    // ############ Per Camera

    TE_PARAM_BLOCK_BEGIN(PerCameraParamDef)
        TE_PARAM_BLOCK_ENTRY(PerCameraData, gCamera)
    TE_PARAM_BLOCK_END

    extern PerCameraParamDef gPerCameraParamDef;

    // ############ Per Instance

    TE_PARAM_BLOCK_BEGIN(PerInstanceParamDef)
        TE_PARAM_BLOCK_ENTRY_ARRAY(PerInstanceData, gInstances, STANDARD_FORWARD_MAX_INSTANCED_BLOCK_SIZE)
    TE_PARAM_BLOCK_END

    extern PerInstanceParamDef gPerInstanceParamDef;
    extern SPtr<GpuParamBlockBuffer> gPerInstanceParamBuffer[STANDARD_FORWARD_MAX_INSTANCED_BLOCKS_NUMBER];

    // ############ Per Material

    TE_PARAM_BLOCK_BEGIN(PerMaterialParamDef)
        TE_PARAM_BLOCK_ENTRY(PerMaterialData, gMaterial)
    TE_PARAM_BLOCK_END

    extern PerMaterialParamDef gPerMaterialParamDef;

    // ############ Per Light

    TE_PARAM_BLOCK_BEGIN(PerLightsParamDef)
        TE_PARAM_BLOCK_ENTRY_ARRAY(PerLightData, gLights, STANDARD_FORWARD_MAX_NUM_LIGHTS)
        TE_PARAM_BLOCK_ENTRY(UINT32, gLightsNumber)
        TE_PARAM_BLOCK_ENTRY(Vector3, gPadding1) // # PADDING
    TE_PARAM_BLOCK_END

    extern PerLightsParamDef gPerLightsParamDef;
    extern SPtr<GpuParamBlockBuffer> gPerLightsParamBuffer;

    // ############ Per Frame

    TE_PARAM_BLOCK_BEGIN(PerFrameParamDef)
        TE_PARAM_BLOCK_ENTRY(float, gTime)
        TE_PARAM_BLOCK_ENTRY(float, gFrameDelta)
        TE_PARAM_BLOCK_ENTRY(Vector2, gPadding2) // # PADDING
        TE_PARAM_BLOCK_ENTRY(UINT32, gUseSkyboxMap)
        TE_PARAM_BLOCK_ENTRY(UINT32, gUseSkyboxDiffuseIrrMap)
        TE_PARAM_BLOCK_ENTRY(UINT32, gUseSkyboxPrefilteredRadianceMap)
        TE_PARAM_BLOCK_ENTRY(UINT32, gSkyboxNumMips)
        TE_PARAM_BLOCK_ENTRY(Vector4, gSceneLightColor)
        TE_PARAM_BLOCK_ENTRY(UINT32, gUseGamma)
        TE_PARAM_BLOCK_ENTRY(UINT32, gUseToneMapping)
        TE_PARAM_BLOCK_ENTRY(float, gGamma)
        TE_PARAM_BLOCK_ENTRY(float, gExposure)
        TE_PARAM_BLOCK_ENTRY(float, gContrast)
        TE_PARAM_BLOCK_ENTRY(float, gBrightness)
        TE_PARAM_BLOCK_ENTRY(float, gSkyboxBrightness)
        TE_PARAM_BLOCK_ENTRY(float, gPadding3) // # PADDING
    TE_PARAM_BLOCK_END

    extern PerFrameParamDef gPerFrameParamDef;

    // ############ Per Object

    TE_PARAM_BLOCK_BEGIN(PerObjectParamDef)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatWorld)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatInvWorld)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatWorldNoScale)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatInvWorldNoScale)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatPrevWorld)
        TE_PARAM_BLOCK_ENTRY(UINT32, gLayer)
        TE_PARAM_BLOCK_ENTRY(UINT32, gHasAnimation)
        TE_PARAM_BLOCK_ENTRY(UINT32, gWriteVelocity)
        TE_PARAM_BLOCK_ENTRY(UINT32, gCastLights)
    TE_PARAM_BLOCK_END

    extern PerObjectParamDef gPerObjectParamDef;

    // ############ Per Call

    TE_PARAM_BLOCK_BEGIN(PerCallParamDef)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatWorldViewProj)
    TE_PARAM_BLOCK_END

    extern PerCallParamDef gPerCallParamDef;

    // ############ Per Decal

    TE_PARAM_BLOCK_BEGIN(DecalParamDef)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gWorldToDecal)
        TE_PARAM_BLOCK_ENTRY(Vector3, gDecalNormal)
        TE_PARAM_BLOCK_ENTRY(float, gNormalTolerance)
        TE_PARAM_BLOCK_ENTRY(float, gFlipDerivatives)
        TE_PARAM_BLOCK_ENTRY(UINT32, gLayerMask)
    TE_PARAM_BLOCK_END

    extern DecalParamDef gDecalParamDef;

    enum class RenderManCulling
    {
        Frustum = 1 << 0,
        Occlusion = 1 << 1
    };

    /** Instancing method for RenderMan */
    enum class RenderManInstancing
    {
        Automatic, /**< We loop each frame through all objects to find those who can be instanced. (CPU cost) */
        Manual, /**< User must set on its own the instancing property for each object */
        None /**< No instancing is used */
    };

    /** Texture filtering options for RenderMan. */
    enum class RenderManFiltering
    {
        Bilinear, /**< Sample linearly in X and Y directions within a texture mip level. */
        Trilinear, /**< Sample bilinearly and also between texture mip levels to hide the mip transitions. */
        Anisotropic /**< High quality dynamic filtering that improves quality of angled surfaces */
    };

    /** Available implementation of the RenderElement class. */
    enum class RenderElementType
    {
        /** See RenderableElement. */
        Renderable,
        /** See ParticlesRenderElement. */
        Particle,
        /** See DecalRenderElement. */
        Decal
    };

    /** Types of ways for shaders to handle MSAA. */
    enum class MSAAMode
    {
        /** No MSAA supported. */
        None,
        /** Single MSAA sample will be resolved. */
        Single,
        /** All MSAA samples will be resolved. */
        Full,
    };

    /** State used to controlling how are properties that need to maintain their previous frame state updated. */
    enum class PrevFrameDirtyState
    {
        /** Most recent version of the property was updated this frame, and its old data stored as prev. version. */
        Updated,
        /** No update has been done this frame, most recent version of the properties should be copied into prev. frame. */
        CopyMostRecent,
        /** Most recent and prev. frame versions are the same and require no updates. */
        Clean
    };

    /** Information about current time and frame index. */
    struct FrameTimings
    {
        float Time = 0.0f;
        float TimeDelta = 0.0f;
        UINT64 FrameIdx = 0;
    };

    struct RenderManOptions;
    struct RendererRenderable;
    struct SceneInfo;
    struct FrameInfo;
    struct RendererDecal;

    class RenderMan;
    class RendererScene;
    class RendererView;
    class RendererViewGroup;
    class RenderCompositor;
    class RendererLight;
    class RenderableElement;
    class DecalRenderElement;
}
