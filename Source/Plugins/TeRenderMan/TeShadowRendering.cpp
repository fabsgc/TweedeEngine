#include "TeShadowRendering.h"

namespace te
{
    void ShadowDepthNormalMat::Bind(const SPtr<GpuParamBlockBuffer>& shadowParams)
    {
        // TODO Shadow
    }

    void ShadowDepthNormalMat::SetPerObjectBuffer(const SPtr<GpuParamBlockBuffer>& perObjectParams)
    {
        // TODO Shadow
    }

    ShadowDepthNormalMat* ShadowDepthNormalMat::GetVariation(bool skinned)
    {
        if (skinned)
            return Get(GetVariation<true>());
        else
            return Get(GetVariation<false>());
    }

    void ShadowDepthCubeMat::Bind(const SPtr<GpuParamBlockBuffer>& shadowParams, const SPtr<GpuParamBlockBuffer>& shadowCubeParams)
    {
        // TODO Shadow
    }

    void ShadowDepthCubeMat::SetPerObjectBuffer(const SPtr<GpuParamBlockBuffer>& perObjectParams, const SPtr<GpuParamBlockBuffer>& shadowCubeMasks)
    {
        // TODO Shadow
    }

    ShadowDepthCubeMat* ShadowDepthCubeMat::GetVariation(bool skinned)
    {
        if (skinned)
            return Get(GetVariation<true>());
        else
            return Get(GetVariation<false>());
    }

    ShadowDepthCubeMat::ShadowDepthCubeMat()
    { }

    const UINT32 ShadowRendering::MAX_ATLAS_SIZE = 4096;
    const UINT32 ShadowRendering::MAX_UNUSED_FRAMES = 60;
    const UINT32 ShadowRendering::MIN_SHADOW_MAP_SIZE = 32;
    const UINT32 ShadowRendering::SHADOW_MAP_FADE_SIZE = 64;
    const UINT32 ShadowRendering::SHADOW_MAP_BORDER = 4;
    const float ShadowRendering::CASCADE_FRACTION_FADE = 0.1f;

    ShadowRendering::ShadowRendering(UINT32 shadowMapSize)
        : _shadowMapSize(shadowMapSize)
    { }

    void ShadowRendering::RenderShadowMaps(RendererScene& scene, const RendererViewGroup& viewGroup, const FrameInfo& frameInfo)
    {
        // TODO Shadow
    }

    void ShadowRendering::SetShadowMapSize(UINT32 size)
    {
        if (_shadowMapSize == size)
            return;

        _shadowMapSize = size;
    }

    void ShadowRendering::RenderCascadedShadowMaps(const RendererView& view, UINT32 lightIdx, RendererScene& scene,
        const FrameInfo& frameInfo)
    {
        // TODO Shadow
    }

    /** Renders shadow maps for the provided spot light. */
    void ShadowRendering::RenderSpotShadowMap(const RendererLight& light, const ShadowMapOptions& options, RendererScene& scene,
        const FrameInfo& frameInfo)
    {
        // TODO Shadow
    }

    /** Renders shadow maps for the provided radial light. */
    void ShadowRendering::RenderRadialShadowMap(const RendererLight& light, const ShadowMapOptions& options, RendererScene& scene,
        const FrameInfo& frameInfo)
    {
        // TODO Shadow
    }
}
