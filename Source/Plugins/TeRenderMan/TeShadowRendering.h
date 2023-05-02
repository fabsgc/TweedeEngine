#pragma once

#include "TeRenderManPrerequisites.h"
#include "Resources/TeBuiltinResources.h"
#include "Renderer/TeRendererMaterial.h"
#include "Renderer/TeParamBlocks.h"
#include "Renderer/TeLight.h"
#include "Math/TeMatrix4.h"

namespace te
{
    TE_PARAM_BLOCK_BEGIN(ShadowParamsDef)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatViewProj)
        TE_PARAM_BLOCK_ENTRY(Vector2, gNDCZToDeviceZ)
        TE_PARAM_BLOCK_ENTRY(float, gDepthBias)
        TE_PARAM_BLOCK_ENTRY(float, gInvDepthRange)
    TE_PARAM_BLOCK_END

    extern ShadowParamsDef gShadowParamsDef;

    /** Material used for rendering a single face of a shadow map, while applying bias in the pixel shader. */
    class ShadowDepthNormalMat : public RendererMaterial<ShadowDepthNormalMat>
    {
        RMAT_DEF(BuiltinShader::ShadowDepthNormal);

        /** Helper method used for initializing variations of this material. */
        template<bool skinned>
        static const ShaderVariation& GetVariation()
        {
            static ShaderVariation variation = ShaderVariation(
            {
                ShaderVariation::Param("SKINNED", skinned)
            });

            return variation;
        }

    public:
        ShadowDepthNormalMat() = default;

        /** Binds the material to the pipeline, ready to be used on subsequent draw calls. */
        void Bind(const SPtr<GpuParamBlockBuffer>& shadowParams);

        /** Sets a new buffer that determines per-object properties. */
        void SetPerObjectBuffer(const SPtr<GpuParamBlockBuffer>& perObjectParams);

        /**
         * Returns the material variation matching the provided parameters.
         *
         * @param[in]	skinned		True if the shadow caster supports bone animation.
         */
        static ShadowDepthNormalMat* GetVariation(bool skinned);
    };

    TE_PARAM_BLOCK_BEGIN(ShadowCubeMatricesDef)
        TE_PARAM_BLOCK_ENTRY_ARRAY(Matrix4, gFaceVPMatrices, 6)
    TE_PARAM_BLOCK_END

    extern ShadowCubeMatricesDef gShadowCubeMatricesDef;

    TE_PARAM_BLOCK_BEGIN(ShadowCubeMasksDef)
        TE_PARAM_BLOCK_ENTRY_ARRAY(int, gFaceMasks, 6)
    TE_PARAM_BLOCK_END

    extern ShadowCubeMasksDef gShadowCubeMasksDef;

    /** Material used for rendering an omni directional cube shadow map. */
    class ShadowDepthCubeMat : public RendererMaterial<ShadowDepthCubeMat>
    {
        RMAT_DEF(BuiltinShader::ShadowDepthCube);

        /** Helper method used for initializing variations of this material. */
        template<bool skinned>
        static const ShaderVariation& GetVariation()
        {
            static ShaderVariation variation = ShaderVariation(
            {
                ShaderVariation::Param("SKINNED", skinned)
            });

            return variation;
        }
    public:
        ShadowDepthCubeMat();

        /** Binds the material to the pipeline, ready to be used on subsequent draw calls. */
        void Bind(const SPtr<GpuParamBlockBuffer>& shadowParams, const SPtr<GpuParamBlockBuffer>& shadowCubeParams);

        /** Sets a new buffer that determines per-object properties. */
        void SetPerObjectBuffer(const SPtr<GpuParamBlockBuffer>& perObjectParams,
            const SPtr<GpuParamBlockBuffer>& shadowCubeMasks);

        /**
         * Returns the material variation matching the provided parameters.
         *
         * @param[in]	skinned		True if the shadow caster supports bone animation.
         */
        static ShadowDepthCubeMat* GetVariation(bool skinned);
    };

    /** Provides functionality for rendering shadow maps. */
    class ShadowRendering
    {
    public:
        /** Contains information required for generating a shadow map for a specific light. */
        struct ShadowMapOptions
        {
            UINT32 LightIdx;
            UINT32 MapSize;
            Vector<float> FadePercents;
        };

    public:
        ShadowRendering(UINT32 shadowMapSize);

        /** For each visible shadow casting light, renders a shadow map from its point of view. */
        void RenderShadowMaps(RendererScene& scene, const RendererViewGroup& viewGroup, const FrameInfo& frameInfo);

        /** Changes the default shadow map size. Will cause all shadow maps to be rebuilt. */
        void SetShadowMapSize(UINT32 size);

    private:
        /** Renders cascaded shadow maps for the provided directional light viewed from the provided view. */
        void RenderCascadedShadowMaps(const RendererView& view, UINT32 lightIdx, RendererScene& scene,
            const FrameInfo& frameInfo);

        /** Renders shadow maps for the provided spot light. */
        void RenderSpotShadowMap(const RendererLight& light, const ShadowMapOptions& options, RendererScene& scene,
            const FrameInfo& frameInfo);

        /** Renders shadow maps for the provided radial light. */
        void RenderRadialShadowMap(const RendererLight& light, const ShadowMapOptions& options, RendererScene& scene,
            const FrameInfo& frameInfo);

    private:
        /** Size of a single shadow map atlas, in pixels. */
        static const UINT32 MAX_ATLAS_SIZE;

        /** Determines how long will an unused shadow map atlas stay allocated, in frames. */
        static const UINT32 MAX_UNUSED_FRAMES;

        /** Determines the minimal resolution of a shadow map. */
        static const UINT32 MIN_SHADOW_MAP_SIZE;

        /** Determines the resolution at which shadow maps begin fading out. */
        static const UINT32 SHADOW_MAP_FADE_SIZE;

        /** Size of the border of a shadow map in a shadow map atlas, in pixels. */
        static const UINT32 SHADOW_MAP_BORDER;

        /** Percent of the length of a single cascade in a CSM, in which to fade out the cascade. */
        static const float CASCADE_FRACTION_FADE;

    private:
        UINT32 _shadowMapSize;
    };
}
