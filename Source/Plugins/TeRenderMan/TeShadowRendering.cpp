#include "TeShadowRendering.h"

#include "TeRendererScene.h"
#include "RenderAPI/TeRenderTexture.h"

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

    void ShadowInfo::UpdateNormArea(UINT32 atlasSize)
    {
        NormArea.x = Area.x / (float)atlasSize;
        NormArea.y = Area.y / (float)atlasSize;
        NormArea.width = Area.width / (float)atlasSize;
        NormArea.height = Area.height / (float)atlasSize;
    }

    ShadowMapBase::ShadowMapBase(UINT32 size)
        : _size(size)
        , _isUsed(false)
        , _lastUsedCounter(0)
    { }

    SPtr<Texture> ShadowMapBase::GetTexture() const
    {
        return _shadowMap->Tex;
    }

    ShadowCubemap::ShadowCubemap(UINT32 size)
        : ShadowMapBase(size)
    {
        _shadowMap = gGpuResourcePool().Get(
            POOLED_RENDER_TEXTURE_DESC::CreateCube(SHADOW_MAP_FORMAT, size, size, TU_DEPTHSTENCIL));
    }

    SPtr<RenderTexture> ShadowCubemap::GetTarget() const
    {
        return _shadowMap->RenderTex;
    }

    ShadowCascadedMap::ShadowCascadedMap(UINT32 size, UINT32 numCascades)
        : ShadowMapBase(size)
        , _numCascades(numCascades)
        , _targets(numCascades)
        , _shadowInfos(numCascades)
    {
        _shadowMap = gGpuResourcePool().Get(POOLED_RENDER_TEXTURE_DESC::Create2D(SHADOW_MAP_FORMAT, size, size,
            TU_DEPTHSTENCIL, 0, false, numCascades));

        RENDER_TEXTURE_DESC rtDesc;
        rtDesc.DepthStencilSurface.Tex = _shadowMap->Tex;
        rtDesc.DepthStencilSurface.NumFaces = 1;

        for (UINT32 i = 0; i < _numCascades; ++i)
        {
            rtDesc.DepthStencilSurface.Face = i;
            _targets[i] = RenderTexture::Create(rtDesc);
        }
    }

    SPtr<RenderTexture> ShadowCascadedMap::GetTarget(UINT32 cascadeIdx) const
    {
        return _targets[cascadeIdx];
    }

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
        const SceneInfo& sceneInfo = scene.GetSceneInfo();
        const VisibilityInfo& visibility = viewGroup.GetVisibilityInfo();

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
