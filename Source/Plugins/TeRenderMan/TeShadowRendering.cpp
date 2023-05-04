#include "TeShadowRendering.h"

#include "TeRendererScene.h"
#include "RenderAPI/TeRenderTexture.h"
#include "Utility/TeBitwise.h"

namespace te
{
    ShadowParamsDef gShadowParamsDef;

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

    ShadowCubeMatricesDef gShadowCubeMatricesDef;
    ShadowCubeMasksDef gShadowCubeMasksDef;

    ShadowDepthCubeMat::ShadowDepthCubeMat()
    { }

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

    void ShadowInfo::UpdateNormArea(UINT32 atlasSize)
    {
        NormArea.x = Area.x / (float)atlasSize;
        NormArea.y = Area.y / (float)atlasSize;
        NormArea.width = Area.width / (float)atlasSize;
        NormArea.height = Area.height / (float)atlasSize;
    }

    ShadowMapAtlas::ShadowMapAtlas(UINT32 size)
        : _layout(0, 0, size, size, true)
        , _lastUsedCounter(0)
    {
        _atlas = gGpuResourcePool().Get(
            POOLED_RENDER_TEXTURE_DESC::Create2D(SHADOW_MAP_FORMAT, size, size, TU_DEPTHSTENCIL));
    }

    bool ShadowMapAtlas::AddMap(UINT32 size, Rect2I& area, UINT32 border)
    {
        UINT32 sizeWithBorder = size + border * 2;

        UINT32 x, y;
        if (!_layout.AddElement(sizeWithBorder, sizeWithBorder, x, y))
            return false;

        area.width = area.height = size;
        area.x = x + border;
        area.y = y + border;

        _lastUsedCounter = 0;
        return true;
    }

    void ShadowMapAtlas::Clear()
    {
        _layout.Clear();
        _lastUsedCounter++;
    }

    bool ShadowMapAtlas::IsEmpty() const
    {
        return _layout.IsEmpty();
    }

    SPtr<Texture> ShadowMapAtlas::GetTexture() const
    {
        return _atlas->Tex;
    }

    SPtr<RenderTexture> ShadowMapAtlas::GetTarget() const
    {
        return _atlas->RenderTex;
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

        // Clear all transient data from last frame
        _shadowInfos.clear();

        _spotLightShadows.resize(sceneInfo.SpotLights.size());
        _radialLightShadows.resize(sceneInfo.RadialLights.size());
        _directionalLightShadows.resize(sceneInfo.DirectionalLights.size());

        _spotLightShadowOptions.clear();
        _radialLightShadowOptions.clear();

        // Clear all dynamic light atlases
        for (auto& entry : _cascadedShadowMaps)
            entry.Clear();

        for (auto& entry : _dynamicShadowMaps)
            entry.Clear();

        for (auto& entry : _shadowCubemaps)
            entry.Clear();

        // Determine shadow map sizes and sort them
        UINT32 shadowInfoCount = 0;
        for (UINT32 i = 0; i < (UINT32)sceneInfo.SpotLights.size(); ++i)
        {
            const RendererLight& light = sceneInfo.SpotLights[i];
            _spotLightShadows[i].StartIdx = shadowInfoCount;
            _spotLightShadows[i].NumShadows = 0;

            // Note: I'm using visibility across all views, while I could be using visibility for every view individually,
            // if I kept that information somewhere
            if (!light._internal->GetCastShadows() || !visibility.SpotLights[i])
                continue;

            ShadowMapOptions options;
            options.LightIdx = i;

            float maxFadePercent;
            CalcShadowMapProperties(light, viewGroup, SHADOW_MAP_BORDER, options.MapSize, options.FadePercents, maxFadePercent);

            // Don't render shadow maps that will end up nearly completely faded out
            if (maxFadePercent < 0.005f)
                continue;

            _spotLightShadowOptions.push_back(options);
            shadowInfoCount++; // For now, always a single fully dynamic shadow for a single light, but that may change
        }

        for (UINT32 i = 0; i < (UINT32)sceneInfo.RadialLights.size(); ++i)
        {
            const RendererLight& light = sceneInfo.RadialLights[i];
            _radialLightShadows[i].StartIdx = shadowInfoCount;
            _radialLightShadows[i].NumShadows = 0;

            // Note: I'm using visibility across all views, while I could be using visibility for every view individually,
            // if I kept that information somewhere
            if (!light._internal->GetCastShadows() || !visibility.RadialLights[i])
                continue;

            ShadowMapOptions options;
            options.LightIdx = i;

            float maxFadePercent;
            CalcShadowMapProperties(light, viewGroup, 0, options.MapSize, options.FadePercents, maxFadePercent);

            // Don't render shadow maps that will end up nearly completely faded out
            if (maxFadePercent < 0.005f)
                continue;

            _radialLightShadowOptions.push_back(options);

            shadowInfoCount++; // For now, always a single fully dynamic shadow for a single light, but that may change
        }

        // Sort spot lights by size so they fit neatly in the texture atlas
        std::sort(_spotLightShadowOptions.begin(), _spotLightShadowOptions.end(),
            [](const ShadowMapOptions& a, const ShadowMapOptions& b) { return a.MapSize > b.MapSize; });

        // Reserve space for shadow infos
        _shadowInfos.resize(shadowInfoCount);

        // Deallocate unused textures (must be done before rendering shadows, in order to ensure indices don't change)
        for (auto iter = _dynamicShadowMaps.begin(); iter != _dynamicShadowMaps.end(); ++iter)
        {
            if (iter->GetLastUsedCounter() >= MAX_UNUSED_FRAMES)
            {
                // These are always populated in order, so we can assume all following atlases are also empty
                _dynamicShadowMaps.erase(iter, _dynamicShadowMaps.end());
                break;
            }
        }

        for (auto iter = _cascadedShadowMaps.begin(); iter != _cascadedShadowMaps.end();)
        {
            if (iter->GetLastUsedCounter() >= MAX_UNUSED_FRAMES)
                iter = _cascadedShadowMaps.erase(iter);
            else
                ++iter;
        }

        for (auto iter = _shadowCubemaps.begin(); iter != _shadowCubemaps.end();)
        {
            if (iter->GetLastUsedCounter() >= MAX_UNUSED_FRAMES)
                iter = _shadowCubemaps.erase(iter);
            else
                ++iter;
        }

        // Render shadow maps
        for (UINT32 i = 0; i < (UINT32)sceneInfo.DirectionalLights.size(); ++i)
        {
            const RendererLight& light = sceneInfo.DirectionalLights[i];

            if (!light._internal->GetCastShadows())
                return;

            UINT32 numViews = viewGroup.GetNumViews();
            _directionalLightShadows[i].ViewShadows.resize(numViews);

            for (UINT32 j = 0; j < numViews; ++j)
                RenderCascadedShadowMaps(*viewGroup.GetView(j), i, scene, frameInfo);
        }

        for (auto& entry : _spotLightShadowOptions)
        {
            UINT32 lightIdx = entry.LightIdx;
            const RendererLight& light = sceneInfo.SpotLights[lightIdx];

            if (!light._internal->GetCastShadows())
                return;

            RenderSpotShadowMap(sceneInfo.SpotLights[lightIdx], entry, scene, frameInfo);
        }

        for (auto& entry : _radialLightShadowOptions)
        {
            UINT32 lightIdx = entry.LightIdx;
            const RendererLight& light = sceneInfo.RadialLights[lightIdx];

            if (!light._internal->GetCastShadows())
                return;
            
            RenderRadialShadowMap(sceneInfo.RadialLights[lightIdx], entry, scene, frameInfo);
        }
    }

    void ShadowRendering::SetShadowMapSize(UINT32 size)
    {
        if (_shadowMapSize == size)
            return;

        _cascadedShadowMaps.clear();
        _dynamicShadowMaps.clear();
        _shadowCubemaps.clear();

        _shadowMapSize = size;
    }

    void ShadowRendering::RenderCascadedShadowMaps(const RendererView& view, UINT32 lightIdx, RendererScene& scene,
        const FrameInfo& frameInfo)
    {
        UINT32 viewIdx = view.GetViewIdx();
        RenderAPI& rapi = RenderAPI::Instance();
        LightShadows& lightShadows = _directionalLightShadows[lightIdx].ViewShadows[viewIdx];

        if (!view.GetRenderSettings().ShadowSettings.Enabled)
        {
            lightShadows.StartIdx = (UINT32) - 1;
            lightShadows.NumShadows = 0;
            return;
        }

        // Note: Currently I'm using spherical bounds for the cascaded frustum which might result in non-optimal usage
        // of the shadow map. A different approach would be to generate a bounding box and then both adjust the aspect
        // ratio (and therefore dimensions) of the shadow map, as well as rotate the camera so the visible area best fits
        // in the map. It remains to be seen if this is viable.
        //  - Note2: Actually both of these will likely have serious negative impact on shadow stability.
        const SceneInfo& sceneInfo = scene.GetSceneInfo();

        const RendererLight& rendererLight = sceneInfo.DirectionalLights[lightIdx];
        Light* light = rendererLight._internal;

        const Transform& tfrm = light->GetTransform();
        Vector3 lightDir = -tfrm.GetRotation().ZAxis();
        SPtr<GpuParamBlockBuffer> shadowParamsBuffer = gShadowParamsDef.CreateBuffer();

        ShadowInfo shadowInfo;
        shadowInfo.LightIdx = lightIdx;
        shadowInfo.TextureIdx = (UINT32)-1;

        UINT32 mapSize = std::min(_shadowMapSize, MAX_ATLAS_SIZE);
        shadowInfo.Area = Rect2I(0, 0, mapSize, mapSize);
        shadowInfo.UpdateNormArea(mapSize);

        UINT32 numCascades = view.GetRenderSettings().ShadowSettings.NumCascades;
        for (UINT32 i = 0; i < (UINT32)_cascadedShadowMaps.size(); i++)
        {
            ShadowCascadedMap& shadowMap = _cascadedShadowMaps[i];

            if (!shadowMap.IsUsed() && shadowMap.GetSize() == mapSize && shadowMap.GetNumCascades() == numCascades)
            {
                shadowInfo.TextureIdx = i;
                shadowMap.MarkAsUsed();

                break;
            }
        }

        if (shadowInfo.TextureIdx == (UINT32)-1)
        {
            shadowInfo.TextureIdx = (UINT32)_cascadedShadowMaps.size();
            _cascadedShadowMaps.push_back(ShadowCascadedMap(mapSize, numCascades));

            ShadowCascadedMap& shadowMap = _cascadedShadowMaps.back();
            shadowMap.MarkAsUsed();
        }

        ShadowCascadedMap& shadowMap = _cascadedShadowMaps[shadowInfo.TextureIdx];

        Quaternion lightRotation(TeIdentity);
        lightRotation.LookRotation(lightDir, Vector3::UNIT_Y);

        for (UINT32 i = 0; i < numCascades; ++i)
        {
            rapi.PushMarker("[DRAW] Project Directional Shadow", Color(0.4f, 0.57f, 0.6f));

            // TODO Shadow

            rapi.PopMarker();
        }

        lightShadows.StartIdx = shadowInfo.TextureIdx;
        lightShadows.NumShadows = 1;
    }

    /** Renders shadow maps for the provided spot light. */
    void ShadowRendering::RenderSpotShadowMap(const RendererLight& rendererLight, const ShadowMapOptions& options, RendererScene& scene,
        const FrameInfo& frameInfo)
    {
        Light* light = rendererLight._internal;
        RenderAPI& rapi = RenderAPI::Instance();
        SPtr<GpuParamBlockBuffer> shadowParamsBuffer = gShadowParamsDef.CreateBuffer();

        ShadowInfo mapInfo;
        mapInfo.FadePerView = options.FadePercents;
        mapInfo.LightIdx = options.LightIdx;
        mapInfo.CascadeIdx = (UINT32)-1;

        bool foundSpace = false;
        for (UINT32 i = 0; i < (UINT32)_dynamicShadowMaps.size(); i++)
        {
            ShadowMapAtlas& atlas = _dynamicShadowMaps[i];

            if (atlas.AddMap(options.MapSize, mapInfo.Area, SHADOW_MAP_BORDER))
            {
                mapInfo.TextureIdx = i;

                foundSpace = true;
                break;
            }
        }

        if (!foundSpace)
        {
            mapInfo.TextureIdx = (UINT32)_dynamicShadowMaps.size();
            _dynamicShadowMaps.push_back(ShadowMapAtlas(MAX_ATLAS_SIZE));

            ShadowMapAtlas& atlas = _dynamicShadowMaps.back();
            atlas.AddMap(options.MapSize, mapInfo.Area, SHADOW_MAP_BORDER);
        }

        mapInfo.UpdateNormArea(MAX_ATLAS_SIZE);
        ShadowMapAtlas& atlas = _dynamicShadowMaps[mapInfo.TextureIdx];

        rapi.PushMarker("[DRAW] Project Spot Shadow", Color(0.4f, 0.57f, 0.6f));

        rapi.SetRenderTarget(atlas.GetTarget());
        rapi.SetViewport(mapInfo.NormArea);
        rapi.ClearViewport(FBT_DEPTH);

        mapInfo.DepthNear = 0.05f;
        mapInfo.DepthFar = 1.f;
        mapInfo.DepthFade = mapInfo.DepthFar;
        mapInfo.FadeRange = 0.f;
        mapInfo.DepthRange = mapInfo.DepthFar - mapInfo.DepthNear;
        mapInfo.DepthBias = GetDepthBias(*light, light->GetBounds().GetRadius(), mapInfo.DepthRange, options.MapSize);
        mapInfo.SubjectBounds = light->GetBounds();

        Quaternion lightRotation = light->GetTransform().GetRotation();

        Matrix4 view = Matrix4::View(rendererLight.GetShiftedLightPosition(), lightRotation);
        Matrix4 proj = Matrix4::ProjectionPerspective(light->GetSpotAngle(), 1.f, 0.05f, 1.f);

        ConvexVolume localFrustum = ConvexVolume(proj);
        rapi.ConvertProjectionMatrix(proj, proj);

        mapInfo.ShadowVPTransform = proj * view;

        gShadowParamsDef.gDepthBias.Set(shadowParamsBuffer, mapInfo.DepthBias);
        gShadowParamsDef.gInvDepthRange.Set(shadowParamsBuffer, 1.0f / mapInfo.DepthRange);
        gShadowParamsDef.gMatViewProj.Set(shadowParamsBuffer, mapInfo.ShadowVPTransform);
        gShadowParamsDef.gNDCZToDeviceZ.Set(shadowParamsBuffer, RendererView::GetNDCZToDeviceZ());

        const Vector<Plane>& frustumPlanes = localFrustum.GetPlanes();
        Matrix4 worldMatrix = view.InverseAffine();

        Vector<Plane> worldPlanes(frustumPlanes.size());
        UINT32 j = 0;
        for (auto& plane : frustumPlanes)
        {
            worldPlanes[j] = worldMatrix.MultiplyAffine(plane);
            j++;
        }

        ConvexVolume worldFrustum(worldPlanes);

        // TODO Shadow

        rapi.SetViewport(Rect2(0.0f, 0.0f, 1.0f, 1.0f));

        LightShadows& lightShadows = _spotLightShadows[options.LightIdx];

        _shadowInfos[lightShadows.StartIdx + lightShadows.NumShadows] = mapInfo;
        lightShadows.NumShadows++;

        rapi.PopMarker();
    }

    /** Renders shadow maps for the provided radial light. */
    void ShadowRendering::RenderRadialShadowMap(const RendererLight& rendererLight, const ShadowMapOptions& options, RendererScene& scene,
        const FrameInfo& frameInfo)
    {
        Light* light = rendererLight._internal;
        RenderAPI& rapi = RenderAPI::Instance();
        SPtr<GpuParamBlockBuffer> shadowParamsBuffer = gShadowParamsDef.CreateBuffer();

        ShadowInfo mapInfo;
        mapInfo.LightIdx = options.LightIdx;
        mapInfo.TextureIdx = (UINT32)-1;
        mapInfo.FadePerView = options.FadePercents;
        mapInfo.CascadeIdx = (UINT32)-1;
        mapInfo.Area = Rect2I(0, 0, options.MapSize, options.MapSize);
        mapInfo.UpdateNormArea(options.MapSize);

        for (UINT32 i = 0; i < (UINT32)_shadowCubemaps.size(); i++)
        {
            ShadowCubemap& cubemap = _shadowCubemaps[i];

            if (!cubemap.IsUsed() && cubemap.GetSize() == options.MapSize)
            {
                mapInfo.TextureIdx = i;
                cubemap.MarkAsUsed();

                break;
            }
        }

        if (mapInfo.TextureIdx == (UINT32)-1)
        {
            mapInfo.TextureIdx = (UINT32)_shadowCubemaps.size();
            _shadowCubemaps.push_back(ShadowCubemap(options.MapSize));

            ShadowCubemap& cubemap = _shadowCubemaps.back();
            cubemap.MarkAsUsed();
        }

        ShadowCubemap& cubemap = _shadowCubemaps[mapInfo.TextureIdx];

        mapInfo.DepthNear = 0.05f;
        mapInfo.DepthFar = 1.f;
        mapInfo.DepthFade = mapInfo.DepthFar;
        mapInfo.FadeRange = 0.0f;
        mapInfo.DepthRange = mapInfo.DepthFar - mapInfo.DepthNear;
        mapInfo.DepthBias = GetDepthBias(*light, light->GetBounds().GetRadius(), mapInfo.DepthRange, options.MapSize);
        mapInfo.SubjectBounds = light->GetBounds();

        const RenderAPICapabilities& caps = gCaps();

        // Note: Projecting on positive Z axis, because cubemaps use a left-handed coordinate system
        Matrix4 proj = Matrix4::ProjectionPerspective(Degree(90.f), 1.f, 0.05f, 1.f, true);
        ConvexVolume localFrustum(proj);

        // Render cubemaps upside down if necessary
        Matrix4 adjustedProj = proj;
        if (caps.Convention.UV_YAxis == Conventions::Axis::Up)
        {
            // All big APIs use the same cubemap sampling coordinates, as well as the same face order. But APIs that
            // use bottom-up UV coordinates require the cubemap faces to be stored upside down in order to get the same
            // behaviour. APIs that use an upside-down NDC Y axis have the same problem as the rendered image will be
            // upside down, but this is handled by the projection matrix. If both of those are enabled, then the effect
            // cancels out.

            adjustedProj[1][1] = -proj[1][1];
        }

        bool renderAllFacesAtOnce = caps.HasCapability(RSC_RENDER_TARGET_LAYERS);

        SPtr<GpuParamBlockBuffer> shadowCubeMatricesBuffer;
        SPtr<GpuParamBlockBuffer> shadowCubeMasksBuffer;
        if (renderAllFacesAtOnce)
        {
            shadowCubeMatricesBuffer = gShadowCubeMatricesDef.CreateBuffer();
            shadowCubeMasksBuffer = gShadowCubeMasksDef.CreateBuffer();
        }

        gShadowParamsDef.gDepthBias.Set(shadowParamsBuffer, mapInfo.DepthBias);
        gShadowParamsDef.gInvDepthRange.Set(shadowParamsBuffer, 1.0f / mapInfo.DepthRange);
        gShadowParamsDef.gMatViewProj.Set(shadowParamsBuffer, Matrix4::IDENTITY);
        gShadowParamsDef.gNDCZToDeviceZ.Set(shadowParamsBuffer, RendererView::GetNDCZToDeviceZ());

        ConvexVolume frustums[6];
        Vector<Plane> boundingPlanes;
        for (UINT32 i = 0; i < 6; i++)
        {
            // Calculate view matrix
            Vector3 forward;
            Vector3 up = Vector3::UNIT_Y;

            switch (i)
            {
            case CF_PositiveX:
                forward = Vector3::UNIT_X;
                break;
            case CF_NegativeX:
                forward = -Vector3::UNIT_X;
                break;
            case CF_PositiveY:
                forward = Vector3::UNIT_Y;
                up = -Vector3::UNIT_Z;
                break;
            case CF_NegativeY:
                forward = -Vector3::UNIT_Y;
                up = Vector3::UNIT_Z;
                break;
            case CF_PositiveZ:
                forward = Vector3::UNIT_Z;
                break;
            case CF_NegativeZ:
                forward = -Vector3::UNIT_Z;
                break;
            }

            Vector3 right = Vector3::Cross(up, forward);
            Matrix3 viewRotationMat = Matrix3(right, up, forward);

            Vector3 lightPos = light->GetTransform().GetPosition();
            Matrix4 viewOffsetMat = Matrix4::Translation(-lightPos);

            Matrix4 view = Matrix4(viewRotationMat.Transpose()) * viewOffsetMat;
            mapInfo.ShadowVPTransforms[i] = proj * view;

            Matrix4 shadowViewProj = adjustedProj * view;

            // Calculate world frustum for culling
            const Vector<Plane>& frustumPlanes = localFrustum.GetPlanes();

            Matrix4 worldMatrix = Matrix4::Translation(lightPos) * Matrix4(viewRotationMat);

            Vector<Plane> worldPlanes(frustumPlanes.size());
            UINT32 j = 0;
            for (auto& plane : frustumPlanes)
            {
                worldPlanes[j] = worldMatrix.MultiplyAffine(plane);
                j++;
            }

            ConvexVolume frustum(worldPlanes);

            if (renderAllFacesAtOnce)
            {
                frustums[i] = frustum;

                // Register far plane of all frustums
                boundingPlanes.push_back(worldPlanes[FRUSTUM_PLANE_FAR]);
                gShadowCubeMatricesDef.gFaceVPMatrices.Set(shadowCubeMatricesBuffer, shadowViewProj, i);
            }
        }

        if (renderAllFacesAtOnce)
        {
            rapi.PushMarker("[DRAW] Project Spot Shadow", Color(0.4f, 0.57f, 0.6f));

            rapi.SetRenderTarget(cubemap.GetTarget());
            rapi.ClearRenderTarget(FBT_DEPTH);

            // TODO Shadow

            rapi.PopMarker();
        }

        LightShadows& lightShadows = _radialLightShadows[options.LightIdx];

        _shadowInfos[lightShadows.StartIdx + lightShadows.NumShadows] = mapInfo;
        lightShadows.NumShadows++;
    }

    void ShadowRendering::CalcShadowMapProperties(const RendererLight& light, const RendererViewGroup& viewGroup,
        UINT32 border, UINT32& size, Vector<float>& fadePercents, float& maxFadePercent) const
    {
        const static float SHADOW_TEXELS_PER_PIXEL = 1.0f;

        // Find a view in which the light has the largest radius
        float maxMapSize = 0.0f;
        maxFadePercent = 0.0f;
        for (int i = 0; i < (int)viewGroup.GetNumViews(); ++i)
        {
            const RendererView& view = *viewGroup.GetView(i);
            const RendererViewProperties& viewProps = view.GetProperties();
            const RenderSettings& viewSettings = view.GetRenderSettings();

            if (!viewSettings.ShadowSettings.Enabled)
                fadePercents.push_back(0.f);
            else
            {
                // Approximation for screen space sphere radius: screenSize * 0.5 * cot(fov) * radius / Z, where FOV is the
                // largest one
                //// First get sphere depth
                const Matrix4& viewVP = viewProps.ViewProjTransform;
                float depth = viewVP.Multiply(Vector4(light._internal->GetTransform().GetPosition(), 1.0f)).w;

                // This is just 1/tan(fov), for both horz. and vert. FOV
                float viewScaleX = viewProps.ProjTransform[0][0];
                float viewScaleY = viewProps.ProjTransform[1][1];

                float screenScaleX = viewScaleX * viewProps.Target.ViewRect.width * 0.5f;
                float screenScaleY = viewScaleY * viewProps.Target.ViewRect.height * 0.5f;

                float screenScale = std::max(screenScaleX, screenScaleY);

                //// Calc radius (clamp if too close to avoid massive numbers)
                float radiusNDC = light._internal->GetBounds().GetRadius() / std::max(depth, 1.0f);

                //// Radius of light bounds in percent of the view surface, multiplied by screen size in pixels
                float radiusScreen = radiusNDC * screenScale;

                float optimalMapSize = SHADOW_TEXELS_PER_PIXEL * radiusScreen;
                maxMapSize = std::max(maxMapSize, optimalMapSize);

                // Determine if the shadow should fade out
                float fadePercent = Math::InvLerp(optimalMapSize, (float)MIN_SHADOW_MAP_SIZE, (float)SHADOW_MAP_FADE_SIZE);
                fadePercents.push_back(fadePercent);
                maxFadePercent = std::max(maxFadePercent, fadePercent);
            }
        }

        // If light fully (or nearly fully) covers the screen, use full shadow map resolution, otherwise
        // scale it down to smaller power of two, while clamping to minimal allowed resolution
        UINT32 effectiveMapSize = Bitwise::NextPow2((UINT32)maxMapSize);
        effectiveMapSize = Math::Clamp(effectiveMapSize, MIN_SHADOW_MAP_SIZE, _shadowMapSize);

        // Leave room for border
        size = std::max(effectiveMapSize - 2 * border, 1u);
    }

    float ShadowRendering::GetDepthBias(const Light& light, float radius, float depthRange, UINT32 mapSize)
    {
        const static float RADIAL_LIGHT_BIAS = 0.005f;
        const static float SPOT_DEPTH_BIAS = 0.01f;
        const static float DIR_DEPTH_BIAS = 0.001f; // In clip space units
        const static float DEFAULT_RESOLUTION = 512.f;

        // Increase bias if map size smaller than some resolution
        float resolutionScale = 1.0f;

        if (light.GetType() != Light::Type::Directional)
            resolutionScale = DEFAULT_RESOLUTION / (float)mapSize;

        // Adjust range because in shader we compare vs. clip space depth
        float rangeScale = 1.0f;
        if (light.GetType() == Light::Type::Spot)
            rangeScale = 1.0f / depthRange;

        const RenderAPICapabilities& caps = gCaps();
        float deviceDepthRange = caps.MaxDepth - caps.MinDepth;

        float defaultBias = 1.0f;
        switch (light.GetType())
        {
        case Light::Type::Directional:
            defaultBias = DIR_DEPTH_BIAS * deviceDepthRange;

            // Use larger bias for further away cascades
            defaultBias *= depthRange * 0.01f;
            break;
        case Light::Type::Radial:
            defaultBias = RADIAL_LIGHT_BIAS;
            break;
        case Light::Type::Spot:
            defaultBias = SPOT_DEPTH_BIAS;
            break;
        default:
            break;
        }

        return defaultBias * light.GetShadowBias() * resolutionScale * rangeScale;
    }
}
