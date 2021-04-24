#include "TeRendererView.h"
#include "Renderer/TeCamera.h"
#include "Renderer/TeRenderable.h"
#include "Renderer/TeRenderSettings.h"
#include "TeRendererScene.h"
#include "TeRenderMan.h"
#include "Material/TeMaterial.h"
#include "Material/TeShader.h"

namespace te
{
    PerCameraParamDef gPerCameraParamDef;

    PerInstanceData RendererView::_instanceDataPool[STANDARD_FORWARD_MAX_INSTANCED_BLOCKS_NUMBER][STANDARD_FORWARD_MAX_INSTANCED_BLOCK_SIZE];
    Vector<InstancedBuffer> RendererView::_instancedBuffersPool(8);

    /** Struct used to compare two instanced buffer */
    bool operator==(const InstancedBuffer& lhs, const InstancedBuffer& rhs)
    {
        size_t lhsSize = lhs.MaterialCount;
        size_t rhsSize = rhs.MaterialCount;

        if (lhs.MeshElem != rhs.MeshElem) return false;
        if (lhsSize != rhsSize) return false;

        if (lhsSize == rhsSize)
        {
            for (size_t i = 0; i < lhsSize; i++)
            {
                if (lhs.Materials[i].get() != rhs.Materials[i].get())
                    return false;
            }
        }

        return true;
    }

    RendererViewProperties::RendererViewProperties(const RENDERER_VIEW_DESC& desc)
        : RendererViewData(desc)
        , FrameIdx(0)
        , Target(desc.Target)
    {
        ProjTransformNoAA = desc.ProjTransform;
        ViewProjTransform = desc.ProjTransform * desc.ViewTransform;
    }

    RendererView::RendererView()
        : _camera(nullptr)
    {
        _paramBuffer = gPerCameraParamDef.CreateBuffer();

        _forwardOpaqueQueue = te_shared_ptr_new<RenderQueue>();
        _forwardTransparentQueue = te_shared_ptr_new<RenderQueue>();
    }

    RendererView::RendererView(const RENDERER_VIEW_DESC& desc)
        : _properties(desc)
        , _camera(desc.SceneCamera)
    {
        _paramBuffer = gPerCameraParamDef.CreateBuffer();
        _properties.PrevViewProjTransform = _properties.ViewProjTransform;

        _forwardOpaqueQueue = te_shared_ptr_new<RenderQueue>(desc.ReductionMode);
        _forwardTransparentQueue = te_shared_ptr_new<RenderQueue>(desc.ReductionMode);

        SetStateReductionMode(desc.ReductionMode);
    }

    RendererView::~RendererView()
    { 
        _instancedBuffersPool.clear();
    }

    void RendererView::SetStateReductionMode(StateReduction reductionMode)
    {
        _forwardOpaqueQueue = te_shared_ptr_new<RenderQueue>(reductionMode);

        StateReduction transparentStateReduction = reductionMode;
        if (transparentStateReduction == StateReduction::Material)
            transparentStateReduction = StateReduction::Distance; // Transparent object MUST be sorted by distance

        _forwardTransparentQueue = te_shared_ptr_new<RenderQueue>(transparentStateReduction);
    }

    void RendererView::SetRenderSettings(const SPtr<RenderSettings>& settings)
    {
        if (_renderSettings == nullptr)
            _renderSettings = te_shared_ptr_new<RenderSettings>();

        if (settings != nullptr)
            *_renderSettings = *settings;

        _compositor.Build(*this, RCNodeFinalResolve::GetNodeId());
    }

    void RendererView::SetTransform(const Vector3& origin, const Vector3& direction, const Matrix4& view,
        const Matrix4& proj, const ConvexVolume& worldFrustum)
    {
        _properties.ViewOrigin = origin;
        _properties.ViewDirection = direction;
        _properties.ViewTransform = view;
        _properties.ProjTransform = proj;
        _properties.ProjTransformNoAA = proj;
        _properties.CullFrustum = worldFrustum;
        _properties.ViewProjTransform = proj * view;
    }

    void RendererView::SetView(const RENDERER_VIEW_DESC& desc)
    {
        _camera = desc.SceneCamera;
        _properties = desc;
        _properties.ProjTransformNoAA = desc.ProjTransform;
        _properties.ViewProjTransform = desc.ProjTransform * desc.ViewTransform;
        _properties.PrevViewProjTransform = _properties.ViewProjTransform;
        _properties.Target = desc.Target;

        SetStateReductionMode(desc.ReductionMode);
    }

    void RendererView::BeginFrame(const FrameInfo& frameInfo)
    {
        bool perViewBufferDirty = false;
        if (_camera)
        {
            const SPtr<Viewport>& viewport = _camera->GetViewport();
            if (viewport)
            {
                UINT32 newTargetWidth = 0;
                UINT32 newTargetHeight = 0;
                if (_properties.Target.Target != nullptr)
                {
                    newTargetWidth = _properties.Target.Target->GetProperties().Width;
                    newTargetHeight = _properties.Target.Target->GetProperties().Height;
                }

                if (newTargetWidth != _properties.Target.TargetWidth ||
                    newTargetHeight != _properties.Target.TargetHeight)
                {
                   _properties.Target.ViewRect = viewport->GetPixelArea();
                    _properties.Target.TargetWidth = newTargetWidth;
                    _properties.Target.TargetHeight = newTargetHeight;

                    perViewBufferDirty = true;
                }
            }
        }

        perViewBufferDirty = true;

        _properties.NeedDynamicEnvMapCompute = false;
        _properties.ProjTransform = _properties.ProjTransformNoAA;
        _properties.ViewProjTransform = _properties.ProjTransform * _properties.ViewTransform;

        if (perViewBufferDirty)
            UpdatePerViewBuffer();

        // Note: inverse view-projection can be cached, it doesn't change every frame
        Matrix4 viewProj = _properties.ProjTransform * _properties.ViewTransform;
        Matrix4 invViewProj = viewProj.Inverse();
        Matrix4 NDCToPrevNDC = _properties.PrevViewProjTransform * invViewProj;

        gPerCameraParamDef.gNDCToPrevNDC.Set(_paramBuffer, NDCToPrevNDC);

        _frameTimings = frameInfo.Timings;

        // Account for auto-exposure taking multiple frames
        if (_redrawThisFrame)
        {
            // Note: Doing this here instead of _notifyNeedsRedraw because we need an up-to-date frame index
            if (_renderSettings->EnableHDR && _renderSettings->EnableAutoExposure)
                _waitingOnAutoExposureFrame = _frameTimings.FrameIdx;
            else
                _waitingOnAutoExposureFrame = std::numeric_limits<UINT64>::max();
        }
    }

    void RendererView::EndFrame()
    {
        // Save view-projection matrix to use for temporal filtering
        _properties.PrevViewProjTransform = _properties.ViewProjTransform;

        // Advance per-view frame index. This is used primarily by temporal rendering effects, and pausing the frame index
        // allows you to freeze the current rendering as is, without temporal artifacts.
        _properties.FrameIdx++;

        _forwardOpaqueQueue->Clear();
        _forwardTransparentQueue->Clear();

        if (_redrawForFrames > 0)
            _redrawForFrames--;

        if (_redrawForSeconds > 0.0f)
            _redrawForSeconds -= _frameTimings.TimeDelta;

        _redrawThisFrame = false;
    }

    void RendererView::DetermineVisible(const Vector<RendererRenderable*>& renderables, const Vector<CullInfo>& cullInfos,
        Vector<RenderableVisibility>* visibility)
    {
        _visibility.Renderables.clear();
        _visibility.Renderables.resize(renderables.size(), RenderableVisibility());

        if (!ShouldDraw3D())
            return;

        CalculateVisibility(cullInfos, _visibility.Renderables);

        if (visibility != nullptr)
        {
            for (UINT32 i = 0; i < (UINT32)renderables.size(); i++)
            {
                bool visible = (*visibility)[i].Visible;
                (*visibility)[i].Visible = visible || _visibility.Renderables[i].Visible;
            }
        }
    }

    void RendererView::DetermineVisible(const Vector<RendererLight>& lights, const Vector<Sphere>* bounds,
        LightType lightType, Vector<bool>* visibility)
    {
        if (!_renderSettings->EnableLighting)
        {
            for (UINT32 i = 0; i < (UINT32)lights.size(); i++)
                (*visibility)[i] = false;
        }

        if (lightType == LightType::Directional)
        {
            if (visibility && _renderSettings->EnableLighting)
                visibility->assign(lights.size(), true);
            else if(visibility)
                visibility->assign(lights.size(), false);

            return;
        }

        Vector<bool>* perViewVisibility;
        if (lightType == LightType::Radial)
        {
            _visibility.RadialLights.clear();
            _visibility.RadialLights.resize(lights.size(), false);

            perViewVisibility = &_visibility.RadialLights;
        }
        else // Spot
        {
            _visibility.SpotLights.clear();
            _visibility.SpotLights.resize(lights.size(), false);

            perViewVisibility = &_visibility.SpotLights;
        }

        if (!ShouldDraw3D())
            return;

        if (_renderSettings->EnableLighting)
            CalculateVisibility(*bounds, *perViewVisibility);

        if (visibility != nullptr)
        {
            for (UINT32 i = 0; i < (UINT32)lights.size(); i++)
            {
                bool visible = (*visibility)[i];
                (*visibility)[i] = visible || (*perViewVisibility)[i];
            }
        }
    }

    void RendererView::CalculateVisibility(const Vector<CullInfo>& cullInfos, Vector<RenderableVisibility>& visibility) const
    {
        UINT64 cameraLayers = _properties.VisibleLayers;
        const ConvexVolume& worldFrustum = _properties.CullFrustum;
        const Vector3& worldCameraPosition = _properties.ViewOrigin;
        float baseCullDistance = _renderSettings->CullDistance;

        for (UINT32 i = 0; i < (UINT32)cullInfos.size(); i++)
        {
            if ((cullInfos[i].Layer & cameraLayers) == 0)
                continue;

            // Do distance culling
            const Sphere& boundingSphere = cullInfos[i].Boundaries.GetSphere();
            const Vector3& worldRenderablePosition = boundingSphere.GetCenter();

            float distanceToCameraSq = worldCameraPosition.SquaredDistance(worldRenderablePosition);
            float correctedCullDistance = cullInfos[i].CullDistanceFactor * baseCullDistance;
            float maxDistanceToCamera = correctedCullDistance + boundingSphere.GetRadius();

            if (distanceToCameraSq > maxDistanceToCamera* maxDistanceToCamera)
                continue;

            // Do frustum culling
            // Note: This is bound to be a bottleneck at some point. When it is ensure that intersect methods use vector
            // operations, as it is trivial to update them. Also consider spatial partitioning.
            if (worldFrustum.Intersects(boundingSphere))
            {
                // More precise with the box
                const AABox& boundingBox = cullInfos[i].Boundaries.GetBox();

                if (worldFrustum.Intersects(boundingBox))
                    visibility[i].Visible = true;
            }
        }
    }

    void RendererView::CalculateVisibility(const Vector<Sphere>& bounds, Vector<bool>& visibility) const
    {
        const ConvexVolume& worldFrustum = _properties.CullFrustum;

        for (UINT32 i = 0; i < (UINT32)bounds.size(); i++)
        {
            if (worldFrustum.Intersects(bounds[i]))
            {
                visibility[i] = true;
                continue;
            }

            if(_properties.ViewOrigin.Distance(bounds[i].GetCenter()) < bounds[i].GetRadius())
                visibility[i] = true;
        }
    }

    void RendererView::CalculateVisibility(const Vector<AABox>& bounds, Vector<bool>& visibility) const
    {
        const ConvexVolume& worldFrustum = _properties.CullFrustum;

        for (UINT32 i = 0; i < (UINT32)bounds.size(); i++)
        {
            if (worldFrustum.Intersects(bounds[i]))
                visibility[i] = true;
        }
    }

    void RendererView::QueueRenderElements(const SceneInfo& sceneInfo)
    {
        // Queue renderables
        for (UINT32 i = 0; i < (UINT32)sceneInfo.Renderables.size(); i++)
        {
            if (!_visibility.Renderables[i].Visible)
                continue;

            const AABox& boundingBox = sceneInfo.RenderableCullInfos[i].Boundaries.GetBox();
            const float distanceToCamera = (_properties.ViewOrigin - boundingBox.GetCenter()).Length();

            for (auto& renderElem : sceneInfo.Renderables[i]->Elements)
            {
                UINT32 shaderFlags = renderElem.MaterialElem->GetShader()->GetFlags();
                UINT32 techniqueIdx = renderElem.DefaultTechniqueIdx;

                // Note: I could keep renderables in multiple separate arrays, so I don't need to do the check here
                if (shaderFlags & (UINT32)ShaderFlag::Transparent)
                    _forwardTransparentQueue->Add(&renderElem, distanceToCamera, techniqueIdx);
                else
                    _forwardOpaqueQueue->Add(&renderElem, distanceToCamera, techniqueIdx);

                CheckIfDynamicEnvMappingNeeded(renderElem);
            }
        }

        _forwardOpaqueQueue->Sort();
        _forwardTransparentQueue->Sort();
    }

    void RendererView::QueueRenderInstancedElements(const SceneInfo& sceneInfo, InstancedBuffer& instancedBuffer)
    {
        // We now have a list of similar objects to render
        // However, each instance can't be bigger than 128 elements
        // So we divide the size of the list by 128 to know how many instance blocks we will render
        UINT32 instBlockCount = ((UINT32)instancedBuffer.Idx.size() / STANDARD_FORWARD_MAX_INSTANCED_BLOCK_SIZE) + 1;
        UINT32 instancedObjectCounter = 0;

        if (instBlockCount > STANDARD_FORWARD_MAX_INSTANCED_BLOCKS_NUMBER)
        {
            instBlockCount = STANDARD_FORWARD_MAX_INSTANCED_BLOCKS_NUMBER;
            TE_DEBUG("Maximum number of instanced block reached : " + ToString(instBlockCount));
        }

        // For each instance block we retrieve all necessary data
        for (UINT32 currInstBlock = 0; currInstBlock < instBlockCount; currInstBlock++)
        {
            // We need to know start and end of element for this instance block
            UINT32 lowerBlockBound = currInstBlock * STANDARD_FORWARD_MAX_INSTANCED_BLOCK_SIZE;
            UINT32 upperBlockBound = (currInstBlock + 1) * STANDARD_FORWARD_MAX_INSTANCED_BLOCK_SIZE;

            if (upperBlockBound > (UINT32)instancedBuffer.Idx.size())
                upperBlockBound = (UINT32)instancedBuffer.Idx.size();

            if (lowerBlockBound == upperBlockBound)
                break;

            // We will use first element of this block for its data (each element has same internal data)
            UINT32 idx = instancedBuffer.Idx[lowerBlockBound];

            const AABox& boundingBox = sceneInfo.RenderableCullInfos[idx].Boundaries.GetBox();
            const float distanceToCamera = (_properties.ViewOrigin - boundingBox.GetCenter()).Length();

            PerInstanceData data;

            for (auto subElemIdx = lowerBlockBound; subElemIdx < upperBlockBound; subElemIdx++)
            {
                UINT32 elemId = instancedBuffer.Idx[subElemIdx];
                const Renderable* renderable = sceneInfo.Renderables[elemId]->RenderablePtr;
                const Matrix4& tfrmNoScale = renderable->GetMatrixNoScale();

                //Once all this stuff is done, we need to write into perinstance buffer
                //GpuParamBlockBuffer* buffer = sceneInfo.Renderables[elemId]->PerObjectParamBuffer.get();

                data.gMatWorld = sceneInfo.Renderables[elemId]->WorldTfrm;
                data.gMatInvWorld = sceneInfo.Renderables[elemId]->WorldTfrm.InverseAffine();
                data.gMatWorldNoScale = tfrmNoScale;
                data.gMatInvWorldNoScale = tfrmNoScale.InverseAffine();
                data.gMatPrevWorld = sceneInfo.Renderables[elemId]->PrevWorldTfrm;
                data.gLayer = (UINT32)renderable->GetLayer();
                data.gHasAnimation = (renderable->IsAnimated()) ? 1 : 0;
                data.gWriteVelocity = (renderable->GetWriteVelocity()) ? 1 : 0;
                data.gCastLight = (renderable->GetCastLight()) ? 1 : 0;

                _instanceDataPool[currInstBlock][subElemIdx - lowerBlockBound] = data;
                instancedObjectCounter++;

                if (instancedObjectCounter > STANDARD_FORWARD_MAX_INSTANCED_BLOCK_SIZE* STANDARD_FORWARD_MAX_INSTANCED_BLOCKS_NUMBER)
                    break;
            }

            // We update per object and per instance buffer
            sceneInfo.Renderables[idx]->UpdatePerInstanceBuffer(_instanceDataPool[currInstBlock], upperBlockBound - lowerBlockBound, currInstBlock);

            // We create all instanced render element using first RendererRenderable data
            for (auto& renderElem : sceneInfo.Renderables[idx]->Elements)
            {
                RenderableElement* elem = te_pool_new<RenderableElement>(false);
                elem->MeshElem = renderElem.MeshElem;
                elem->SubMeshElem = renderElem.SubMeshElem;
                elem->MaterialElem = renderElem.MaterialElem;
                elem->AnimationId = renderElem.AnimationId;
                elem->AnimType = renderElem.AnimType;
                elem->DefaultTechniqueIdx = renderElem.DefaultTechniqueIdx;
                elem->Type = renderElem.Type;
                elem->InstanceCount = (UINT32)(upperBlockBound - lowerBlockBound);

                elem->GpuParamsElem.resize(renderElem.GpuParamsElem.size());
                std::copy(renderElem.GpuParamsElem.begin(), renderElem.GpuParamsElem.end(), elem->GpuParamsElem.data());

                UINT32 shaderFlags = renderElem.MaterialElem->GetShader()->GetFlags();
                UINT32 techniqueIdx = renderElem.DefaultTechniqueIdx;

                _instancedElements.push_back(elem);

                // Note: I could keep renderables in multiple separate arrays, so I don't need to do the check here
                if (shaderFlags & (UINT32)ShaderFlag::Transparent)
                    _forwardTransparentQueue->Add(elem, distanceToCamera, techniqueIdx);
                else
                    _forwardOpaqueQueue->Add(elem, distanceToCamera, techniqueIdx);

                for (auto& gpuParams : renderElem.GpuParamsElem)
                    gpuParams->SetParamBlockBuffer("PerInstanceBuffer", gPerInstanceParamBuffer[currInstBlock]);

                CheckIfDynamicEnvMappingNeeded(renderElem);
            }

            if (instancedObjectCounter > STANDARD_FORWARD_MAX_INSTANCED_BLOCK_SIZE* STANDARD_FORWARD_MAX_INSTANCED_BLOCKS_NUMBER)
                break;
        }
    }

    Matrix4 invertProjectionMatrix(const Matrix4& mat)
    {
        // Try to solve the most common case using high percision calculations, in order to reduce depth error
        if (mat[0][1] == 0.0f && mat[0][3] == 0.0f &&
            mat[1][0] == 0.0f && mat[1][3] == 0.0f &&
            mat[2][0] == 0.0f && mat[2][1] == 0.0f &&
            mat[3][0] == 0.0f && mat[3][1] == 0.0f &&
            mat[3][2] == -1.0f && mat[3][3] == 0.0f)
        {
            double a = mat[0][0];
            double b = mat[1][1];
            double c = mat[2][2];
            double d = mat[2][3];
            double s = mat[0][2];
            double t = mat[1][2];

            return Matrix4(
                (float)(1.0 / a), 0.0f, 0.0f, (float)(-s / a),
                0.0f, (float)(1.0 / b), 0.0f, (float)(-t / b),
                0.0f, 0.0f, 0.0f, -1.0f,
                0.0f, 0.0f, (float)(1.0 / d), (float)(c / d)
            );
        }
        else
        {
            return mat.Inverse();
        }
    }

    void RendererView::UpdatePerViewBuffer()
    {
        Matrix4 viewProj = _properties.ProjTransform * _properties.ViewTransform;
        Matrix4 invProj = invertProjectionMatrix(_properties.ProjTransform);
        Matrix4 invView = _properties.ViewTransform.InverseAffine();
        Matrix4 invViewProj = invView * invProj;
        Matrix4 NDCToPrevNDC = _properties.PrevViewProjTransform * invViewProj;

        gPerCameraParamDef.gMatProj.Set(_paramBuffer, _properties.ProjTransform);
        gPerCameraParamDef.gMatView.Set(_paramBuffer, _properties.ViewTransform);
        gPerCameraParamDef.gMatViewProj.Set(_paramBuffer, viewProj);
        gPerCameraParamDef.gMatPrevViewProj.Set(_paramBuffer, _properties.PrevViewProjTransform);

        gPerCameraParamDef.gNDCToPrevNDC.Set(_paramBuffer, NDCToPrevNDC);
        gPerCameraParamDef.gViewDir.Set(_paramBuffer, _properties.ViewDirection);
        gPerCameraParamDef.gViewOrigin.Set(_paramBuffer, _properties.ViewOrigin);

        Vector4 ndcToUV = GetNDCToUV();
        gPerCameraParamDef.gClipToUVScaleOffset.Set(_paramBuffer, ndcToUV);

        Vector4 uvToNDC(
            1.0f / ndcToUV.x,
            1.0f / ndcToUV.y,
            -ndcToUV.z / ndcToUV.x,
            -ndcToUV.w / ndcToUV.y);
        gPerCameraParamDef.gUVToClipScaleOffset.Set(_paramBuffer, uvToNDC);
    }

    Vector4 RendererView::GetNDCToUV() const
    {
        static const RenderAPICapabilities& caps = gCaps();
        const Rect2I& viewRect = _properties.Target.ViewRect;

        float halfWidth = viewRect.width * 0.5f;
        float halfHeight = viewRect.height * 0.5f;

        float rtWidth = _properties.Target.TargetWidth != 0 ? (float)_properties.Target.TargetWidth : 20.0f;
        float rtHeight = _properties.Target.TargetHeight != 0 ? (float)_properties.Target.TargetHeight : 20.0f;

        Vector4 ndcToUV;
        ndcToUV.x = halfWidth / rtWidth;
        ndcToUV.y = -halfHeight / rtHeight;
        ndcToUV.z = viewRect.x / rtWidth + (halfWidth + caps.HorizontalTexelOffset) / rtWidth;
        ndcToUV.w = viewRect.y / rtHeight + (halfHeight + caps.VerticalTexelOffset) / rtHeight;

        // Either of these flips the Y axis, but if they're both true they cancel out
       if ((caps.Convention.UV_YAxis == Conventions::Axis::Up) ^ (caps.Convention.NDC_YAxis == Conventions::Axis::Down))
            ndcToUV.y = -ndcToUV.y;

        return ndcToUV;
    }

    bool RendererView::ShouldDraw() const
    {
        if (!_properties.OnDemand)
            return true;

        if (_redrawThisFrame)
            return true;

        if (_renderSettings->EnableHDR && _renderSettings->EnableAutoExposure)
        {
            constexpr float AUTO_EXPOSURE_TOLERANCE = 0.01f;

            // The view was redrawn but we still haven't received the eye adaptation results from the GPU, so
            // we keep redrawing until we do
            if (_waitingOnAutoExposureFrame != std::numeric_limits<UINT64>::max())
                return true;

            // Need to render until the auto-exposure reaches the target exposure
            float eyeAdaptationDiff = Math::Abs(_currentEyeAdaptation - _previousEyeAdaptation);
            if (eyeAdaptationDiff > AUTO_EXPOSURE_TOLERANCE)
                return true;
        }

        // TODO need to be used with auto exposure
        // return _redrawForFrames > 0 || _redrawForSeconds > 0.0f;
        return false;
    }

    bool RendererView::ShouldDraw3D() const
    { 
        return !_renderSettings->OverlayOnly && ShouldDraw(); 
    }

    void RendererView::_notifyNeedsRedraw()
    {
        _redrawThisFrame = true;

        // If doing async animation there is a one frame delay
        _redrawForFrames = 1;

        // This will be set once we get the new luminance data from the GPU
        _redrawForSeconds = 0.0f;
    }

    RendererViewGroup::RendererViewGroup(RendererView** views, UINT32 numViews, SPtr<RenderManOptions> options)
        : _options(options)
    {
        SetViews(views, numViews);
    }

    void RendererViewGroup::SetViews(RendererView** views, UINT32 numViews)
    {
        _views.clear();

        for (UINT32 i = 0; i < numViews; i++)
        {
            _views.push_back(views[i]);
            views[i]->_setViewIdx(i);
        }
    }

    void RendererViewGroup::DetermineVisibility(const SceneInfo& sceneInfo)
    {
        const auto numViews = (UINT32)_views.size();

        // Early exit if no views render scene geometry
        bool anyViewsNeed3DDrawing = false;
        for (UINT32 i = 0; i < numViews; i++)
        {
            if (_views[i]->ShouldDraw3D())
            {
                anyViewsNeed3DDrawing = true;
                break;
            }
        }

        if (!anyViewsNeed3DDrawing)
            return;

        // Calculate renderable visibility per view
        _visibility.Renderables.resize(sceneInfo.Renderables.size(), RenderableVisibility());
        _visibility.Renderables.assign(sceneInfo.Renderables.size(), RenderableVisibility());

        for (UINT32 i = 0; i < numViews; i++)
        {
            _views[i]->DetermineVisible(sceneInfo.Renderables, sceneInfo.RenderableCullInfos, &_visibility.Renderables);
        }

        // Calculate light visibility for all views
        const auto numRadialLights = (UINT32)sceneInfo.RadialLights.size();
        _visibility.RadialLights.resize(numRadialLights, false);
        _visibility.RadialLights.assign(numRadialLights, false);

        const auto numSpotLights = (UINT32)sceneInfo.SpotLights.size();
        _visibility.SpotLights.resize(numSpotLights, false);
        _visibility.SpotLights.assign(numSpotLights, false);

        const auto numDirectionalLights = (UINT32)sceneInfo.DirectionalLights.size();
        _visibility.DirectionalLights.resize(numDirectionalLights, false);
        _visibility.DirectionalLights.assign(numDirectionalLights, false);

        for (UINT32 i = 0; i < numViews; i++)
        {
            if (!_views[i]->ShouldDraw3D())
                continue;

            _views[i]->DetermineVisible(sceneInfo.RadialLights, &sceneInfo.RadialLightWorldBounds, LightType::Radial,
                &_visibility.RadialLights);

            _views[i]->DetermineVisible(sceneInfo.SpotLights, &sceneInfo.SpotLightWorldBounds, LightType::Spot,
                &_visibility.SpotLights);

            _views[i]->DetermineVisible(sceneInfo.DirectionalLights, nullptr, LightType::Directional,
                &_visibility.DirectionalLights);
        }

        // Organize light visibility information in a more GPU friendly manner

        // Note: I'm determining light visibility for the entire group. It might be more performance
        // efficient to do it per view. Additionally I'm using a single GPU buffer to hold their information, which is
        // then updated when each view group is rendered. It might be better to keep one buffer reserved per-view.
        _visibleLightData.Update(sceneInfo, *this);
    }

    void RendererViewGroup::SetAllObjectsAsVisible(const SceneInfo& sceneInfo)
    {
        const auto numViews = (UINT32)_views.size();

        // Early exit if no views render scene geometry
        bool anyViewsNeed3DDrawing = false;
        for (UINT32 i = 0; i < numViews; i++)
        {
            if (_views[i]->ShouldDraw3D())
            {
                anyViewsNeed3DDrawing = true;
                break;
            }
        }

        if (!anyViewsNeed3DDrawing)
            return;

        // Calculate renderable visibility per view
        _visibility.Renderables.resize(sceneInfo.Renderables.size(), RenderableVisibility());
        _visibility.Renderables.assign(sceneInfo.Renderables.size(), RenderableVisibility());

        for (UINT32 i = 0; i < (UINT32)_visibility.Renderables.size(); i++)
        {
            _visibility.Renderables[i].Visible = true;
        }
    }

    void RendererViewGroup::GenerateInstanced(const SceneInfo& sceneInfo, RenderManInstancing instancingMode)
    {
        InstancedBuffer key;

        auto PopulateInstanceBuffer = [&](Renderable* renderable, UINT32 current)
        {
            if (!renderable->GetInstancing())
                return;

            key.MeshElem = renderable->GetMesh().get();
            key.Materials = renderable->GetMaterialsPtr();
            key.MaterialCount = renderable->GetNumMaterials();
            if(key.Idx.size() > 0) key.Idx.clear();

            auto iter = find(RendererView::_instancedBuffersPool.begin(), RendererView::_instancedBuffersPool.end(), key);

            if (iter == RendererView::_instancedBuffersPool.end())
            {
                key.Idx.reserve(32);
                key.Idx.push_back(current);
                RendererView::_instancedBuffersPool.push_back(key);
            }
            else
                iter->Idx.push_back(current);
        };

        if (instancingMode == RenderManInstancing::Automatic)
        {
            const auto numRenderables = (UINT32)sceneInfo.Renderables.size();
            RendererView::_instancedBuffersPool.clear();

            // We will separate renderables based on <Material*> and <Renderable*>
            for (UINT32 i = 0; i < numRenderables; i++)
            {
                if (i > _visibility.Renderables.size())
                    continue;

                if (!_visibility.Renderables[sceneInfo.Renderables[i]->RenderablePtr->GetRendererId()].Visible &&
                    (_options->CullingFlags & (UINT32)RenderManCulling::Frustum ||
                        _options->CullingFlags & (UINT32)RenderManCulling::Occlusion))
                {
                    continue;
                }

                PopulateInstanceBuffer(sceneInfo.Renderables[i]->RenderablePtr, i);
            }
        }
        else if (instancingMode == RenderManInstancing::Manual)
        {
            RendererView::_instancedBuffersPool.clear();

            // We will separate renderables based on <Material*> and <Renderable*>
            for (auto& renderable : sceneInfo.RenderablesInstanced)
            {
                if (renderable->RenderablePtr->GetRendererId() > _visibility.Renderables.size() - 1)
                    continue;

                if (!_visibility.Renderables[renderable->RenderablePtr->GetRendererId()].Visible &&
                    (_options->CullingFlags & (UINT32)RenderManCulling::Frustum ||
                        _options->CullingFlags & (UINT32)RenderManCulling::Occlusion))
                {
                    continue;
                }

                PopulateInstanceBuffer(renderable->RenderablePtr, renderable->RenderablePtr->GetRendererId());
            }
        }
    }

    void RendererViewGroup::GenerateRenderQueue(const SceneInfo& sceneInfo, RendererView& view, RenderManInstancing instancingMode)
    {
        if (instancingMode == RenderManInstancing::Automatic || instancingMode == RenderManInstancing::Manual)
        {
            const UINT32 maxInstElement = STANDARD_FORWARD_MAX_INSTANCED_BLOCK_SIZE * STANDARD_FORWARD_MAX_INSTANCED_BLOCKS_NUMBER;
            UINT32 totalInstElem = 0;

            for (auto& element : view._instancedElements)
                te_pool_delete<RenderableElement>(static_cast<RenderableElement*>(element));

            view._instancedElements.clear();

            for (auto& instancedBuffer : RendererView::_instancedBuffersPool)
            {
                totalInstElem += ((UINT32)instancedBuffer.Idx.size() / STANDARD_FORWARD_MAX_INSTANCED_BLOCK_SIZE + 1) * STANDARD_FORWARD_MAX_INSTANCED_BLOCK_SIZE;
                if (totalInstElem > maxInstElement)
                {
                    UINT32 amountInstToRemove = totalInstElem - maxInstElement;
                    instancedBuffer.Idx.resize(instancedBuffer.Idx.size() - amountInstToRemove);
                }

                bool hasTransparentElement = false;

                for (UINT32 i = 0; i < instancedBuffer.MaterialCount; i++)
                {
                    if (!instancedBuffer.Materials[i])
                        continue;

                    UINT32 shaderFlags = instancedBuffer.Materials[i]->GetShader()->GetFlags();
                    if (shaderFlags & (UINT32)ShaderFlag::Transparent)
                        hasTransparentElement = true;
                }

                if (instancedBuffer.Idx.size() >= STANDARD_FORWARD_MIN_INSTANCED_BLOCK_SIZE && !hasTransparentElement)
                {
                    for (auto& idx : instancedBuffer.Idx)
                    {
                        if (idx < view._visibility.Renderables.size()) // When onDemand, view are not fill with renderables
                        {
                            view._visibility.Renderables[idx].Visible = false;
                            view._visibility.Renderables[idx].Instanced = true;
                        }
                    }

                    view.QueueRenderInstancedElements(sceneInfo, instancedBuffer);
                }

                if (totalInstElem > maxInstElement)
                    break;
            }

            if (view.ShouldDraw3D())
                view.QueueRenderElements(sceneInfo);
        }
        else
        {
            if (view.ShouldDraw3D())
                view.QueueRenderElements(sceneInfo);
        }
    }

    bool RendererView::RequiresVelocityWrites() const
    {
        return _renderSettings->MotionBlur.Enabled;
    }

    float RendererView::GetCurrentExposure() const
    {
        if (_renderSettings->EnableAutoExposure)
            return _previousEyeAdaptation; // TODO

        return Math::Pow(2.0f, _renderSettings->ExposureScale);
    }

    void RendererView::CheckIfDynamicEnvMappingNeeded(const RenderElement& element)
    {
        if(element.MaterialElem->GetProperties().UseDynamicEnvironmentMap)
            _properties.NeedDynamicEnvMapCompute = true;
    }
}
