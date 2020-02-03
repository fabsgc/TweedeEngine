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
        _forwardOpaqueQueue = te_shared_ptr_new<RenderQueue>();
        _forwardTransparentQueue = te_shared_ptr_new<RenderQueue>();
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
        Vector<bool>* visibility)
    {
        _visibility.Renderables.clear();
        _visibility.Renderables.resize(renderables.size(), false);

        if (!ShouldDraw3D())
            return;

        CalculateVisibility(cullInfos, _visibility.Renderables);

        if (visibility != nullptr)
        {
            for (UINT32 i = 0; i < (UINT32)renderables.size(); i++)
            {
                bool visible = (*visibility)[i];
                (*visibility)[i] = visible || _visibility.Renderables[i];
            }
        }
    }

    void RendererView::CalculateVisibility(const Vector<CullInfo>& cullInfos, Vector<bool>& visibility) const
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
                    visibility[i] = true;
            }
        }
    }

    void RendererView::QueueRenderElements(const SceneInfo& sceneInfo)
    {
        // Queue renderables
        for (UINT32 i = 0; i < (UINT32)sceneInfo.Renderables.size(); i++)
        {
            if (!_visibility.Renderables[i])
                continue;

            const AABox& boundingBox = sceneInfo.RenderableCullInfos[i].Boundaries.GetBox();
            const float distanceToCamera = (_properties.ViewOrigin - boundingBox.GetCenter()).Length();

            for (auto& renderElem : sceneInfo.Renderables[i]->Elements)
            {
                UINT32 shaderFlags = renderElem.MaterialElem->GetShader()->GetFlags();

                // Note: I could keep renderables in multiple separate arrays, so I don't need to do the check here
                if (shaderFlags & (UINT32)ShaderFlag::Transparent)
                    _forwardTransparentQueue->Add(&renderElem, distanceToCamera);
                else
                    _forwardOpaqueQueue->Add(&renderElem, distanceToCamera);
            }
        }

        _forwardOpaqueQueue->Sort();
        _forwardTransparentQueue->Sort();
    }

    void RendererView::UpdatePerViewBuffer()
    {
        Matrix4 viewProj = _properties.ProjTransform * _properties.ViewTransform;

        gPerCameraParamDef.gMatProj.Set(_paramBuffer, _properties.ProjTransform);
        gPerCameraParamDef.gMatView.Set(_paramBuffer, _properties.ViewTransform);
        gPerCameraParamDef.gMatViewProj.Set(_paramBuffer, viewProj);

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

    RendererViewGroup::RendererViewGroup(RendererView** views, UINT32 numViews)
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
        _visibility.Renderables.resize(sceneInfo.Renderables.size(), false);
        _visibility.Renderables.assign(sceneInfo.Renderables.size(), false);

        for (UINT32 i = 0; i < numViews; i++)
        {
            _views[i]->DetermineVisible(sceneInfo.Renderables, sceneInfo.RenderableCullInfos, &_visibility.Renderables);
        }

        // Generate render queues per camera
        for (UINT32 i = 0; i < numViews; i++)
        {
            if (_views[i]->ShouldDraw3D())
                _views[i]->QueueRenderElements(sceneInfo);
        }
    }
}
