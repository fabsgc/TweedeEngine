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
        Vector3 gViewDir;
        UINT32  gViewportX;
        Vector3 gViewOrigin;
        UINT32  gViewportY;
        Matrix4 gMatViewProj;
        Matrix4 gMatView;
        Matrix4 gMatProj;
        Matrix4 gMatPrevViewProj;
        Matrix4 gNDCToPrevNDC;
        Vector4 gClipToUVScaleOffset;
        Vector4 gUVToClipScaleOffset;
    };

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

    struct PerMaterialData
    {
        Vector4 gAlbedo;
    };

    struct PerLightData
    {
        Vector3 gColor;
        float   gType;
        Vector3 gPosition;
        float   gIntensity;
        Vector3 gDirection;
        float   gAttenuationRadius;
        Vector3 gSpotAngles;
        float   gBoundsRadius;
        float   gLinearAttenuation;
        float   gQuadraticAttenuation;
        Vector2 gPadding;
    };

    TE_PARAM_BLOCK_BEGIN(PerCameraParamDef)
        TE_PARAM_BLOCK_ENTRY(PerCameraData, gCamera)
    TE_PARAM_BLOCK_END

    extern PerCameraParamDef gPerCameraParamDef;

    TE_PARAM_BLOCK_BEGIN(PerInstanceParamDef)
        TE_PARAM_BLOCK_ENTRY_ARRAY(PerInstanceData, gInstances, STANDARD_FORWARD_MAX_INSTANCED_BLOCK_SIZE)
    TE_PARAM_BLOCK_END

    extern PerInstanceParamDef gPerInstanceParamDef;
    extern SPtr<GpuParamBlockBuffer> gPerInstanceParamBuffer[STANDARD_FORWARD_MAX_INSTANCED_BLOCKS_NUMBER];

    TE_PARAM_BLOCK_BEGIN(PerMaterialParamDef)
        TE_PARAM_BLOCK_ENTRY(PerMaterialData, gMaterial)
    TE_PARAM_BLOCK_END

    extern PerMaterialParamDef gPerMaterialParamDef;

    TE_PARAM_BLOCK_BEGIN(PerLightsParamDef)
        TE_PARAM_BLOCK_ENTRY_ARRAY(PerLightData, gLights, STANDARD_FORWARD_MAX_NUM_LIGHTS)
        TE_PARAM_BLOCK_ENTRY(INT32, gLightsNumber)
    TE_PARAM_BLOCK_END

    extern PerLightsParamDef gPerLightsParamDef;
    extern SPtr<GpuParamBlockBuffer> gPerLightsParamBuffer;

    TE_PARAM_BLOCK_BEGIN(PerFrameParamDef)
        TE_PARAM_BLOCK_ENTRY(float, gTime)
        TE_PARAM_BLOCK_ENTRY(float, gFrameDelta)
        TE_PARAM_BLOCK_ENTRY(INT32, gUseSkyboxMap)
        TE_PARAM_BLOCK_ENTRY(INT32, gUseSkyboxIrradianceMap)
        TE_PARAM_BLOCK_ENTRY(Vector4, gSceneLightColor)
        TE_PARAM_BLOCK_ENTRY(float, gSkyboxBrightness)
    TE_PARAM_BLOCK_END

    extern PerFrameParamDef gPerFrameParamDef;

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

    extern PerObjectParamDef gPerObjectParamDef;

    TE_PARAM_BLOCK_BEGIN(PerCallParamDef)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatWorldViewProj)
    TE_PARAM_BLOCK_END

    extern PerCallParamDef gPerCallParamDef;

    TE_PARAM_BLOCK_BEGIN(DecalParamDef)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gWorldToDecal)
        TE_PARAM_BLOCK_ENTRY(Vector3, gDecalNormal)
        TE_PARAM_BLOCK_ENTRY(float, gNormalTolerance)
        TE_PARAM_BLOCK_ENTRY(float, gFlipDerivatives)
        TE_PARAM_BLOCK_ENTRY(INT32, gLayerMask)
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

    class RenderCompositor;
    struct RenderManOptions;
    class RenderMan;
    class RendererScene;
    class RendererView;
    class RendererViewGroup;
    struct PerLightData;
    class RendererLight;
    class RenderableElement;
    struct RendererRenderable;
    struct SceneInfo;
    struct FrameInfo;
    struct RendererDecal;
    class DecalRenderElement;
}
