#include "TeRendererView.h"
#include "Renderer/TeCamera.h"
#include "Renderer/TeRenderable.h"
#include "Renderer/TeRenderSettings.h"

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
    }

    RendererView::RendererView(const RENDERER_VIEW_DESC& desc)
        : _properties(desc)
        , _camera(desc.SceneCamera)
    {
        _paramBuffer = gPerCameraParamDef.CreateBuffer();
    }

    void RendererView::SetRenderSettings(const SPtr<RenderSettings>& settings)
    {
        if (_renderSettings == nullptr)
            _renderSettings = te_shared_ptr_new<RenderSettings>();

        if (settings != nullptr)
            *_renderSettings = *settings;
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

    void RendererView::BeginFrame()
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
    }

    void RendererView::EndFrame()
    {
        // Advance per-view frame index. This is used primarily by temporal rendering effects, and pausing the frame index
        // allows you to freeze the current rendering as is, without temporal artifacts.
        //_properties.FrameIdx++;

        _redrawThisFrame = false;
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

    void RendererView::_notifyNeedsRedraw()
    {
        _redrawThisFrame = true;
    }
}
