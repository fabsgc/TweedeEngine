#pragma once

#include "TeRenderManPrerequisites.h"
#include "Renderer/TeRenderer.h"
#include "Renderer/TeRenderSettings.h"
#include "Renderer/TeLight.h"
#include "Math/TeBounds.h"
#include "Math/TeRect2I.h"
#include "Math/TeRect2.h"
#include "Math/TeConvexVolume.h"
#include "Renderer/TeParamBlocks.h"
#include "Renderer/TeRenderQueue.h"

namespace te
{
    struct SceneInfo;
    struct FrameInfo;

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
        void BeginFrame(const FrameInfo& frameInfo);

        /** Ends rendering and frees any acquired resources. */
        void EndFrame();

        /**
         * Populates view render queues by determining visible renderable objects.
         *
         * @param[in]	renderables			A set of renderable objects to iterate over and determine visibility for.
         * @param[in]	cullInfos			A set of world bounds & other information relevant for culling the provided
         *									renderable objects. Must be the same size as the @p renderables array.
         * @param[out]	visibility			Output parameter that will have the true bit set for any visible renderable
         *									object. If the bit for an object is already set to true, the method will never
         *									change it to false which allows the same bitfield to be provided to multiple
         *									renderer views. Must be the same size as the @p renderables array.
         */
        void DetermineVisible(const Vector<RendererRenderable*>& renderables, const Vector<CullInfo>& cullInfos,
            Vector<bool>* visibility = nullptr);

        /**
         * Culls the provided set of bounds against the current frustum and outputs a set of visibility flags determining
         * which entry is or isn't visible by this view. Both inputs must be arrays of the same size.
         */
        void CalculateVisibility(const Vector<CullInfo>& cullInfos, Vector<bool>& visibility) const;

        /**
         * Inserts all visible renderable elements into render queues. Assumes visibility has been calculated beforehand
         * by calling determineVisible(). After the call render elements can be retrieved from the queues using
         * getOpaqueQueue or getTransparentQueue() calls.
         */
        void QueueRenderElements(const SceneInfo& sceneInfo);

        /** Updates the GPU buffer containing per-view information, with the latest internal data. */
        void UpdatePerViewBuffer();

        /** Returns a buffer that stores per-view parameters. */
        SPtr<GpuParamBlockBuffer> GetPerViewBuffer() const { return _paramBuffer; }

        /** Assigns a view index to the view. To be called by the parent view group when the view is added to it. */
        void _setViewIdx(UINT32 viewIdx) { _viewIdx = viewIdx; }

        /** Returns an index of this view within the parent view group. */
        UINT32 GetViewIdx() const { return _viewIdx; }

        /** Determines if a view should be rendered this frame. */
        bool ShouldDraw() const;

        /** Determines if view's 3D geometry should be rendered this frame. */
        bool ShouldDraw3D() const;

        /** Lets an on-demand view know that it should be redrawn this frame. */
        void _notifyNeedsRedraw();

    private:
        RendererViewProperties _properties;
        Camera* _camera;

        SPtr<RenderSettings> _renderSettings;
        SPtr<GpuParamBlockBuffer> _paramBuffer;

        VisibilityInfo _visibility;
        UINT32 _viewIdx = 0;

        // On-demand drawing
        float _redrawForSeconds = 0.0f;
        UINT32 _redrawForFrames = 0;
        bool _redrawThisFrame = false;

        // Current frame info
        FrameTimings _frameTimings;

        SPtr<RenderQueue> _forwardOpaqueQueue;
    };

    /** Contains one or multiple RendererView%s that are in some way related. */
    class RendererViewGroup
    {
    public:
        RendererViewGroup(RendererView** views, UINT32 numViews);

        /**
         * Updates the internal list of views. This is more efficient than always constructing a new instance of this class
         * when views change, as internal buffers don't need to be re-allocated.
         */
        void SetViews(RendererView** views, UINT32 numViews);

        /** Returns a view at the specified index. Index must be less than the value returned by getNumViews(). */
        RendererView* GetView(UINT32 idx) const { return _views[idx]; }

        /** Returns the total number of views in the group. */
        UINT32 GetNumViews() const { return (UINT32)_views.size(); }

        /**
         * Returns information about visibility of various scene objects, from the perspective of all the views in the
         * group (visibility will be true if the object is visible from any of the views. determineVisibility() must be
         * called whenever the scene or view information changes (usually every frame).
         */
        const VisibilityInfo& GetVisibilityInfo() const { return _visibility; }

        /**
         * Updates visibility information for the provided scene objects, from the perspective of all views in this group,
         * and updates the render queues of each individual view. Use getVisibilityInfo() to retrieve the calculated
         * visibility information.
         */
        void DetermineVisibility(const SceneInfo& sceneInfo);

    private:
        Vector<RendererView*> _views;
        VisibilityInfo _visibility;
    };
}
