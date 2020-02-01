#pragma once

#include "TeRenderManPrerequisites.h"
#include "Renderer/TeRenderer.h"
#include "Renderer/TeLight.h"
#include "Math/TeBounds.h"
#include "Math/TeRect2I.h"
#include "Math/TeRect2.h"
#include "Math/TeConvexVolume.h"
#include "Renderer/TeParamBlocks.h"

namespace te
{
    TE_PARAM_BLOCK_BEGIN(PerCameraParamDef)
        TE_PARAM_BLOCK_ENTRY(Vector3, gViewDir)
        TE_PARAM_BLOCK_ENTRY(Vector3, gViewOrigin)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatViewProj)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatView)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatProj)
    TE_PARAM_BLOCK_END

    /** Data shared between RENDERER_VIEW_DESC and RendererViewProperties */
    struct RendererViewData
    {
        Matrix4 ViewTransform;
        Matrix4 ProjTransform;
        Vector3 ViewDirection;
        Vector3 ViewOrigin;
        bool FlipView;
        float NearPlane;
        float FarPlane;
        ProjectionType ProjType;

        /**
         * Determines does this view output to the final render target. If false the view is usually used for some
         * sort of helper rendering.
         */
        bool MainView;

        /** If true the view will only be rendered when requested, otherwise it will be rendered every frame. */
        ConvexVolume CullFrustum;

        /** If true the view will only be rendered when requested, otherwise it will be rendered every frame. */
        bool OnDemand;
    };

    /** Data shared between RENDERER_VIEW_TARGET_DESC and RendererViewTargetProperties */
    struct RendererViewTargetData
    {
        SPtr<RenderTarget> Target;

        Rect2I ViewRect;
        Rect2 NrmViewRect;
        UINT32 TargetWidth;
        UINT32 TargetHeight;
        UINT32 NumSamples;

        UINT32 ClearFlags;
        Color ClearColor;
        float ClearDepthValue;
        UINT16 ClearStencilValue;
    };

    /** Set of properties describing the output render target used by a renderer view. */
    struct RENDERER_VIEW_TARGET_DESC : RendererViewTargetData
    { };

    /** Set of properties used describing a specific view that the renderer can render. */
    struct RENDERER_VIEW_DESC : RendererViewData
    {
        RENDERER_VIEW_TARGET_DESC Target;
        Camera* SceneCamera;
    };

    /** Set of properties used describing a specific view that the renderer can render. */
    struct RendererViewProperties : RendererViewData
    {
        RendererViewProperties() {}
        RendererViewProperties(const RENDERER_VIEW_DESC& src);

        Matrix4 ViewProjTransform;
        Matrix4 ProjTransformNoAA;
        UINT32 FrameIdx;

        RendererViewTargetData Target;
    };

    struct VisibilityInfo
    {
        Vector<bool> Renderables;
    };

    /**	Renderer information specific to a single render target. */
    struct RendererRenderTarget
    {
        SPtr<RenderTarget> Target = nullptr;
        Vector<Camera*> Cameras;
    };

    /** Information used for culling an object against a view. */
    struct CullInfo
    {
        CullInfo(const Bounds& bounds, float cullDistanceFactor = 1.0f)
            : Boundaries(bounds)
            , CullDistanceFactor(cullDistanceFactor)
        { }

        Bounds Boundaries;
        float CullDistanceFactor;
    };

    /** Contains information about a single view into the scene, used by the renderer. */
    class RendererView
    {
    public:
        RendererView();
        RendererView(const RENDERER_VIEW_DESC& desc);

        /** Updates the internal camera render settings. */
        void SetRenderSettings(const SPtr<RenderSettings>& settings);

        /** Returns per-view settings that control rendering. */
        const RenderSettings& GetRenderSettings() const { return *_renderSettings; }

        /** Updates the internal information with a new view transform. */
        void SetTransform(const Vector3& origin, const Vector3& direction, const Matrix4& view,
            const Matrix4& proj, const ConvexVolume& worldFrustum);

        /** Updates all internal information with new view information. */
        void SetView(const RENDERER_VIEW_DESC& desc);

        /** Returns a structure describing the view. */
        const RendererViewProperties& GetProperties() const { return _properties; }

        /** Returns the scene camera this object is based of. This can be null for manually constructed renderer cameras. */
        Camera* GetSceneCamera() const { return _camera; }

        /** Prepares render targets for rendering. When done call endFrame(). */
        void BeginFrame();

        /** Ends rendering and frees any acquired resources. */
        void EndFrame();

        /** Updates the GPU buffer containing per-view information, with the latest internal data. */
        void UpdatePerViewBuffer();

        /** Returns a buffer that stores per-view parameters. */
        SPtr<GpuParamBlockBuffer> GetPerViewBuffer() const { return _paramBuffer; }

        /** Lets an on-demand view know that it should be redrawn this frame. */
        void _notifyNeedsRedraw();

    private:
        RendererViewProperties _properties;
        Camera* _camera;

        SPtr<RenderSettings> _renderSettings;
        SPtr<GpuParamBlockBuffer> _paramBuffer;

        VisibilityInfo _visibility;

        // On-demand drawing
        bool _redrawThisFrame = false;
    };
}
