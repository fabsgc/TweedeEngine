#pragma once

#include "TeRenderManPrerequisites.h"
#include "Resources/TeBuiltinResources.h"
#include "Renderer/TeRendererMaterial.h"
#include "Renderer/TeGpuResourcePool.h"
#include "Renderer/TeParamBlocks.h"
#include "Renderer/TeLight.h"
#include "Math/TeMatrix4.h"
#include "Math/TeRect2I.h"
#include "Math/TeRect2.h"
#include "Image/TePixelData.h"

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

    /** Pixel format used for rendering and storing shadow maps. */
    const PixelFormat SHADOW_MAP_FORMAT = PF_D16;

    /** Information about a shadow cast from a single light. */
    struct ShadowInfo
    {
        /** Updates normalized area coordinates based on the non-normalized ones and the provided atlas size. */
        void UpdateNormArea(UINT32 atlasSize);

        UINT32 LightIdx; /**< Index of the light casting this shadow. */
        Rect2I Area; /**< Area of the shadow map in pixels, relative to its source texture. */
        Rect2 NormArea; /**< Normalized shadow map area in [0, 1] range. */
        UINT32 TextureIdx; /**< Index of the texture the shadow map is stored in. */

        float DepthNear; /**< Distance to the near plane. */
        float DepthFar; /**< Distance to the far plane. */
        float DepthFade; /**< Distance to the plane at which to start fading out the shadows (only for CSM). */
        float FadeRange; /**< Distance from the fade plane to the far plane (only for CSM). */

        float DepthBias; /**< Bias used to reduce shadow acne. */
        float DepthRange; /**< Length of the range covered by the shadow caster volume. */

        UINT32 CascadeIdx; /**< Index of a cascade. Only relevant for CSM. */

        /** View-projection matrix from the shadow casters point of view. */
        Matrix4 ShadowVPTransform;

        /** View-projection matrix for each cubemap face, used for omni-directional shadows. */
        Matrix4 ShadowVPTransforms[6];

        /** Bounds of the geometry the shadow is being applied on. */
        Sphere SubjectBounds;

        /** Determines the fade amount of the shadow, for each view in the scene. */
        Vector<float> fadePerView;
    };

    /**
     * Contains a texture that serves as an atlas for one or multiple shadow maps. Provides methods for inserting new maps
     * in the atlas.
     */
    class ShadowMapAtlas
    {
    public:
        // TODO Shadows
    };

    /** Contains common code for different shadow map types. */
    class ShadowMapBase
    {
    public:
        ShadowMapBase(UINT32 size);
        virtual ~ShadowMapBase() {}

        /** Returns the bindable shadow map texture. */
        SPtr<Texture> GetTexture() const;

        /** Returns the size of a single face of the shadow map texture, in pixels. */
        UINT32 GetSize() const { return _size; }

        /** Makes the shadow map available for re-use and increments the counter returned by getLastUsedCounter(). */
        void Clear() { _isUsed = false; _lastUsedCounter++; }

        /** Marks the shadow map as used and resets the last used counter to zero. */
        void MarkAsUsed() { _isUsed = true; _lastUsedCounter = 0; }

        /** Returns true if the object is storing a valid shadow map. */
        bool IsUsed() const { return _isUsed; }

        /**
         * Returns the value of the last used counter. See MarkAsUsed() for information on how is
         * the counter incremented/decremented.
         */
        UINT32 GetLastUsedCounter() const { return _lastUsedCounter; }
       
    protected:
    protected:
        SPtr<PooledRenderTexture> _shadowMap;
        UINT32 _size;

        bool _isUsed;
        UINT32 _lastUsedCounter;
    };

    /** Contains a cubemap for storing an omnidirectional cubemap. */
    class ShadowCubemap : public ShadowMapBase
    {
    public:
        ShadowCubemap(UINT32 size);

        /** Returns a render target encompassing all six faces of the shadow cubemap. */
        SPtr<RenderTexture> GetTarget() const;
    };

    /** Contains a texture required for rendering cascaded shadow maps. */
    class ShadowCascadedMap : public ShadowMapBase
    {
    public:
        ShadowCascadedMap(UINT32 size, UINT32 numCascades);

        /** Returns the total number of cascades in the cascade shadow map. */
        UINT32 GetNumCascades() const { return _numCascades; }

        /** Returns a render target that allows rendering into a specific cascade of the cascaded shadow map. */
        SPtr<RenderTexture> GetTarget(UINT32 cascadeIdx) const;

        /** Provides information about a shadow for the specified cascade. */
        void SetShadowInfo(UINT32 cascadeIdx, const ShadowInfo& info) { _shadowInfos[cascadeIdx] = info; }

        /** @copydoc setShadowInfo */
        const ShadowInfo& GetShadowInfo(UINT32 cascadeIdx) const { return _shadowInfos[cascadeIdx]; }

    private:
        UINT32 _numCascades;
        Vector<SPtr<RenderTexture>> _targets;
        Vector<ShadowInfo> _shadowInfos;
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

        /** Contains references to all shadows cast by a specific light. */
        struct LightShadows
        {
            UINT32 StartIdx = 0;
            UINT32 NumShadows = 0;
        };

        /** Contains references to all shadows cast by a specific light, per view. */
        struct PerViewLightShadows
        {
            Vector<LightShadows> ViewShadows;
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

        Vector<ShadowMapAtlas> _dynamicShadowMaps;
        Vector<ShadowCascadedMap> _cascadedShadowMaps;
        Vector<ShadowCubemap> _shadowCubemaps;

        Vector<ShadowInfo> _shadowInfos;

        Vector<LightShadows> _spotLightShadows;
        Vector<LightShadows> _radialLightShadows;
        Vector<PerViewLightShadows> _directionalLightShadows;

        Vector<bool> _renderableVisibility; // Transient
        Vector<ShadowMapOptions> _spotLightShadowOptions; // Transient
        Vector<ShadowMapOptions> _radialLightShadowOptions; // Transient
    };
}
