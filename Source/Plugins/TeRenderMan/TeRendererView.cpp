#include "TeRendererView.h"
#include "Renderer/TeCamera.h"
#include "Renderer/TeRenderable.h"
#include "Renderer/TeRenderSettings.h"
#include "TeRendererScene.h"
#include "TeRenderMan.h"
#include "TeRendererRenderable.h"
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

    RendererViewProperties::RendererViewProperties(const RENDERER_VIEW_DESC& src)
        : RendererViewData(src)
        , FrameIdx(0)
        , Target(src.Target)
    {
        ProjTransformNoAA = src.ProjTransform;
        ViewProjTransform = src.ProjTransform * src.ViewTransform;
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

        if (perViewBufferDirty)
            UpdatePerViewBuffer();

        _frameTimings = frameInfo.Timings;
    }

    void RendererView::EndFrame()
    {
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

    void RendererView::CalculateVisibility(const Vector<CullInfo>& cullInfos, Vector<RenderableVisibility>& visibility) const
    {
        UINT64 cameraLayers = _properties.VisibleLayers;
        const ConvexVolume& worldFrustum = _properties.CullFrustum;
        const Vector3& worldCameraPosition = _properties.ViewOrigin;
        float baseCullDistance = _renderSettings->CullDistance;

        for (UINT32 i = 0; i < (UINT32)cullInfos.size(); i++)
        {
            visibility[i].Visible = true;
            continue;

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
            // TE_DEBUG("Maximum number of instanced block reached : " + ToString(instBlockCount), __FILE__, __LINE__);
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

                //Once all this stuff is done, we need to write into perinstance buffer
                GpuParamBlockBuffer* buffer = sceneInfo.Renderables[elemId]->PerObjectParamBuffer.get();

                data.gMatWorld = gPerObjectParamDef.gMatWorld.Get(buffer);
                data.gMatInvWorld = gPerObjectParamDef.gMatInvWorld.Get(buffer);
                data.gMatWorldNoScale = gPerObjectParamDef.gMatWorldNoScale.Get(buffer);
                data.gMatInvWorldNoScale = gPerObjectParamDef.gMatInvWorldNoScale.Get(buffer);
                data.gMatPrevWorld = gPerObjectParamDef.gMatPrevWorld.Get(buffer);
                data.gLayer = gPerObjectParamDef.gLayer.Get(buffer);

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
                SPtr<RenderableElement> elem = te_shared_ptr_new<RenderableElement>(); // TODO, need a pool of object
                elem->MeshElem = renderElem.MeshElem;
                elem->SubMeshElem = renderElem.SubMeshElem;
                elem->MaterialElem = renderElem.MaterialElem;
                elem->DefaultTechniqueIdx = renderElem.DefaultTechniqueIdx;
                elem->Type = renderElem.Type;
                elem->InstanceCount = (UINT32)(upperBlockBound - lowerBlockBound);
                elem->GpuParamsElem = renderElem.GpuParamsElem;

                UINT32 shaderFlags = renderElem.MaterialElem->GetShader()->GetFlags();
                UINT32 techniqueIdx = renderElem.DefaultTechniqueIdx;

                _instancedElements.push_back(elem);

                // Note: I could keep renderables in multiple separate arrays, so I don't need to do the check here
                if (shaderFlags & (UINT32)ShaderFlag::Transparent)
                    _forwardTransparentQueue->Add(elem.get(), distanceToCamera, techniqueIdx);
                else
                    _forwardOpaqueQueue->Add(elem.get(), distanceToCamera, techniqueIdx);

                for (auto& gpuParams : renderElem.GpuParamsElem)
                {
                    gpuParams->SetParamBlockBuffer("PerInstanceBuffer", gPerInstanceParamBuffer[currInstBlock]);
                }
            }

            if (instancedObjectCounter > STANDARD_FORWARD_MAX_INSTANCED_BLOCK_SIZE* STANDARD_FORWARD_MAX_INSTANCED_BLOCKS_NUMBER)
                break;
        }
    }

    void RendererView::UpdatePerViewBuffer()
    {
        Matrix4 viewProj = _properties.ProjTransform * _properties.ViewTransform;

        gPerCameraParamDef.gMatProj.Set(_paramBuffer, _properties.ProjTransform.Transpose());
        gPerCameraParamDef.gMatView.Set(_paramBuffer, _properties.ViewTransform.Transpose());
        gPerCameraParamDef.gMatViewProj.Set(_paramBuffer, viewProj.Transpose());

        gPerCameraParamDef.gViewDir.Set(_paramBuffer, _properties.ViewDirection);
        gPerCameraParamDef.gViewOrigin.Set(_paramBuffer, _properties.ViewOrigin);
    }

    bool RendererView::ShouldDraw() const
    {
        if (!_properties.OnDemand)
            return true;

        return _redrawForFrames > 0 || _redrawForSeconds > 0.0f;
    }

    bool RendererView::ShouldDraw3D() const
    { 
        return !_renderSettings->OverlayOnly && ShouldDraw(); 
    }

    void RendererView::_notifyNeedsRedraw()
    {
        _redrawThisFrame = true;
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
            const UINT32 maxInstElement = STANDARD_FORWARD_MAX_INSTANCED_BLOCK_SIZE * STANDARD_FORWARD_MAX_INSTANCED_BLOCK_SIZE;
            UINT32 totalInstElem = 0;

            RendererView::_instancedBuffersPool.clear();

            // We will separate renderables based on <Material*> and <Renderable*>
            for (UINT32 i = 0; i < numRenderables; i++)
            {
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
            const auto numRenderables = (UINT32)sceneInfo.RenderablesInstanced.size();
            const UINT32 maxInstElement = STANDARD_FORWARD_MAX_INSTANCED_BLOCK_SIZE * STANDARD_FORWARD_MAX_INSTANCED_BLOCK_SIZE;
            UINT32 totalInstElem = 0;

            RendererView::_instancedBuffersPool.clear();

            // We will separate renderables based on <Material*> and <Renderable*>
            for (auto& renderable : sceneInfo.RenderablesInstanced)
            {
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
            const auto numRenderables = (UINT32)sceneInfo.Renderables.size();
            const UINT32 maxInstElement = STANDARD_FORWARD_MAX_INSTANCED_BLOCK_SIZE * STANDARD_FORWARD_MAX_INSTANCED_BLOCKS_NUMBER;
            UINT32 totalInstElem = 0;

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

                if (instancedBuffer.Idx.size() > STANDARD_FORWARD_MIN_INSTANCED_BLOCK_SIZE && !hasTransparentElement)
                {
                    for (auto& idx : instancedBuffer.Idx)
                    {
                        view._visibility.Renderables[idx].Visible = false;
                        view._visibility.Renderables[idx].Instanced = true;
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
}
