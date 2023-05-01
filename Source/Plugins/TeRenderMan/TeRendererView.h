#pragma once

#include "TeRenderManPrerequisites.h"
#include "TeRendererLight.h"
#include "TeRendererDecal.h"
#include "TeRendererRenderable.h"
#include "Renderer/TeRenderer.h"
#include "Renderer/TeRenderQueue.h"
#include "Math/TeBounds.h"
#include "Math/TeRect2I.h"
#include "Math/TeRect2.h"
#include "Math/TeConvexVolume.h"
#include "Utility/TePoolAllocator.h"

namespace te
{
    /** Data shared between RENDERER_VIEW_DESC and RendererViewProperties */
    struct RendererViewData
    {
        Matrix4 ViewTransform = Matrix4::IDENTITY;
        Matrix4 PrevViewProjTransform = Matrix4::IDENTITY;
        Matrix4 ProjTransform = Matrix4::IDENTITY;
        Vector3 ViewDirection = Vector3::ZERO;
        Vector3 ViewOrigin = Vector3::ZERO;
        bool FlipView = false;
        float NearPlane = 0.0f;
        float FarPlane = 0.0f;
        ProjectionType ProjType = ProjectionType::PT_PERSPECTIVE;

        /**
         * Determines does this view output to the final render target. If false the view is usually used for some
         * sort of helper rendering.
         */
        bool MainView = false;

        /** If true the view will only be rendered when requested, otherwise it will be rendered every frame. */
        bool OnDemand = false;

        UINT64 VisibleLayers = 0;
        ConvexVolume CullFrustum;

        /** When enabled, post-processing effects (like tonemapping) will be executed. */
        bool RunPostProcessing = false;
    };

    /** Data shared between RENDERER_VIEW_TARGET_DESC and RendererViewTargetProperties */
    struct RendererViewTargetData
    {
        SPtr<RenderTarget> Target;

        Rect2I ViewRect;
        Rect2 NrmViewRect;
        UINT32 TargetWidth = 0;
        UINT32 TargetHeight = 0;
        UINT32 NumSamples = 0;

        UINT32 ClearFlags = 0;
        Color ClearColor;
        float ClearDepthValue = 0.0f;
        UINT16 ClearStencilValue = 0;
    };

    /** Set of properties describing the output render target used by a renderer view. */
    struct RENDERER_VIEW_TARGET_DESC : RendererViewTargetData
    { };

    /** Set of properties used describing a specific view that the renderer can render. */
    struct RENDERER_VIEW_DESC : RendererViewData
    {
        RENDERER_VIEW_TARGET_DESC Target;
        Camera* SceneCamera;

        StateReduction ReductionMode;
    };

    /** Set of properties used describing a specific view that the renderer can render. */
    struct RendererViewProperties : RendererViewData
    {
        RendererViewProperties() {}
        RendererViewProperties(const RENDERER_VIEW_DESC& src);

        Matrix4 ViewProjTransform = Matrix4::IDENTITY;
        Matrix4 ProjTransformNoAA = Matrix4::IDENTITY;
        UINT32 FrameIdx = 0;

        RendererViewTargetData Target;
    };

    struct RenderableVisibility
    {
        bool Visible = false;
        bool Instanced = false;
    };

    struct VisibilityInfo
    {
        /* Say if a renderable is currently visible or not */
        Vector<RenderableVisibility> Renderables;
        Vector<bool> DirectionalLights;
        Vector<bool> RadialLights;
        Vector<bool> SpotLights;
    };

    /**	Renderer information specific to a single render target. */
    struct RendererRenderTarget
    {
        SPtr<RenderTarget> Target = nullptr;
        Vector<Camera*> Cameras;
    };

    /** Struct used to store elements that can be instanced in renderQueue */
    struct InstancedBuffer
    {
        Mesh* MeshElem;
        const SPtr<Material>* Materials;
        UINT32 MaterialCount = 0;
        UINT32 Layer = 0;
        Vector<UINT32> Idx;
    };

    /** Information used for culling an object against a view. */
    struct CullInfo
    {
        CullInfo(const Bounds& bounds, UINT64 layer = -1, float cullDistanceFactor = 1.0f)
            : Layer(layer)
            , Boundaries(bounds)
            , CullDistanceFactor(cullDistanceFactor)
        { }

        UINT64 Layer;
        Bounds Boundaries;
        float CullDistanceFactor;
    };

    /** Contains information about a single view into the scene, used by the renderer. */
    class RendererView
    {
    public:
        RendererView();
        RendererView(const RENDERER_VIEW_DESC& desc);
        ~RendererView();

        /** Sets state reduction mode that determines how do render queues group & sort renderables. */
        void SetStateReductionMode(StateReduction reductionMode);

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
         * Returns a render queue containing all opaque objects for the specified pipeline. Make sure to call
         * determineVisible() beforehand if view or object transforms changed since the last time it was called. If @p
         * forward is true then opaque objects using the forward pipeline are returned, otherwise deferred pipeline objects
         * are returned.
         */
        const SPtr<RenderQueue>& GetOpaqueQueue() const { return _forwardOpaqueQueue; }

        /**
         * Returns a render queue containing all transparent objects. Make sure to call determineVisible() beforehand if
         * view or object transforms changed since the last time it was called.
         */
        const SPtr<RenderQueue>& GetTransparentQueue() const { return _forwardTransparentQueue; }

        /** Returns the compositor in charge of rendering for this view. */
        const RenderCompositor& GetCompositor() const;

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
            Vector<RenderableVisibility>* visibility = nullptr);

        /**
         * Calculates the visibility masks for all the lights of the provided type.
         *
         * @param[in]	lights				A set of lights to determine visibility for.
         * @param[in]	bounds				Bounding sphere for each provided light. Must be the same size as the @p lights
         *									array.
         * @param[in]	type				Type of all the lights in the @p lights array.
         * @param[out]	visibility			Output parameter that will have the true bit set for any visible light. If the
         *									bit for a light is already set to true, the method will never change it to false
         *									which allows the same bitfield to be provided to multiple renderer views. Must
         *									be the same size as the @p lights array.
         *
         *									As a side-effect, per-view visibility data is also calculated and can be
         *									retrieved by calling getVisibilityMask().
         */
        void DetermineVisible(const Vector<RendererLight>& lights, const Vector<Sphere>* bounds, LightType type,
            Vector<bool>* visibility = nullptr);

        /**
         * Culls the provided set of bounds against the current frustum and outputs a set of visibility flags determining
         * which entry is or isn't visible by this view. Both inputs must be arrays of the same size.
         */
        void CalculateVisibility(const Vector<CullInfo>& cullInfos, Vector<RenderableVisibility>& visibility) const;

        /**
         * Culls the provided set of bounds against the current frustum and outputs a set of visibility flags determining
         * which entry is or isn't visible by this view. Both inputs must be arrays of the same size.
         */
        void CalculateVisibility(const Vector<Sphere>& bounds, Vector<bool>& visibility) const;

        /**
         * Culls the provided set of bounds against the current frustum and outputs a set of visibility flags determining
         * which entry is or isn't visible by this view. Both inputs must be arrays of the same size.
         */
        void CalculateVisibility(const Vector<AABox>& bounds, Vector<bool>& visibility) const;

        /**
         * Inserts all visible renderable elements into render queues. Assumes visibility has been calculated beforehand
         * by calling determineVisible(). After the call render elements can be retrieved from the queues using
         * getOpaqueQueue or getTransparentQueue() calls.
         */
        void QueueRenderElements(const SceneInfo& sceneInfo);

        /**
         * Inserts all visible instanced renderable elements into render queues. Assumes visibility has been calculated beforehand
         * by calling determineVisible(). After the call render elements can be retrieved from the queues using
         * getOpaqueQueue or getTransparentQueue() calls.
         */
        void QueueRenderInstancedElements(const SceneInfo& sceneInfo, InstancedBuffer& instancedBuffers);

        /** Returns the visibility mask calculated with the last call to determineVisible(). */
        const VisibilityInfo& GetVisibilityInfo() const { return _visibility; }

        /** Updates the GPU buffer containing per-view information, with the latest internal data. */
        void UpdatePerViewBuffer();

        /**
         * Returns a value that can be used for transforming x, y coordinates from NDC into UV coordinates that can be used
         * for sampling a texture projected on the view.
         *
         * @return	Returns two 2D values that can be used to transform the coordinate as such: UV = NDC * xy + zw.
         */
        Vector4 GetNDCToUV() const;

        /** Returns a buffer that stores per-view parameters. */
        SPtr<GpuParamBlockBuffer> GetPerViewBuffer() const { return _paramBuffer; }

        /** Assigns a view index to the view. To be called by the parent view group when the view is added to it. */
        void SetViewIdx(UINT32 viewIdx) { _viewIdx = viewIdx; }

        /** Returns an index of this view within the parent view group. */
        UINT32 GetViewIdx() const { return _viewIdx; }

        /** Determines if a view should be rendered this frame. */
        bool ShouldDraw() const;

        /** Determines if view's 3D geometry should be rendered this frame. */
        bool ShouldDraw3D() const;

        /** Lets an on-demand view know that it should be redrawn this frame. */
        void NotifyNeedsRedraw();

        /**
         * Notifies the view that the render target the compositor is rendering to has changed. Note that this does not
         * mean the final render target, rather the current intermediate target as set by the renderer during the
         * rendering of a single frame. This should be set to null if the renderer is not currently rendering the
         * view.
         */
        void NotifyCompositorTargetChanged(const SPtr<RenderTarget>& target) const { _context.CurrentTarget = target; }

        /** Returns true if the view should write to the velocity buffer. */
        bool RequiresVelocityWrites() const;

        /**
         * Gets the current exposure of the view, used for transforming scene light values from HDR in a range that can be
         * displayed on a display device.
         */
        float GetCurrentExposure() const;

        /** Returns a context that reflects the state of the view as it changes during rendering. */
        const RendererViewContext& GetContext() const { return _context; }

        /**
         * Extracts the necessary values from the projection matrix that allow you to transform device Z value (range [0, 1]
         * into view Z value.
         *
         * @param[in]	projMatrix	Projection matrix that was used to create the device Z value to transform.
         * @return					Returns two values that can be used to transform device z to view z using this formula:
         * 							z = (deviceZ + y) * x.
         */
        static Vector2 GetDeviceZToViewZ(const Matrix4& projMatrix);

        /**
         * Extracts the necessary values from the projection matrix that allow you to transform NDC Z value (range depending
         * on render API) into view Z value.
         *
         * @param[in]	projMatrix	Projection matrix that was used to create the NDC Z value to transform.
         * @return					Returns two values that can be used to transform NDC z to view z using this formula:
         * 							z = (NDCZ + y) * x.
         */
        static Vector2 GetNDCZToViewZ(const Matrix4& projMatrix);

        /**
         * Returns a value that can be used for tranforming a depth value in NDC, to a depth value in device Z ([0, 1]
         * range using this formula: (NDCZ + y) * x.
         */
        static Vector2 GetNDCZToDeviceZ();

    private:
        friend class RendererViewGroup;
        friend class Renderable;

        RendererViewProperties _properties;
        mutable RendererViewContext _context;
        Camera* _camera;

        UPtr<RenderCompositor> _compositor;
        SPtr<RenderSettings> _renderSettings;
        SPtr<GpuParamBlockBuffer> _paramBuffer;

        VisibilityInfo _visibility;
        UINT32 _viewIdx = 0;

        // On-demand drawing 
        // _redrawForFrames, _redrawForSeconds and _waitingOnAutoExposureFrame are not used because I don't manage auto exposure yet
        // TODO need to be used with auto exposure
        float _redrawForSeconds = 0.0f;
        UINT32 _redrawForFrames = 0;
        bool _redrawThisFrame = false;
        UINT64 _waitingOnAutoExposureFrame = std::numeric_limits<UINT64>::max();

        // Current frame info
        FrameTimings _frameTimings;

        SPtr<RenderQueue> _forwardOpaqueQueue;
        SPtr<RenderQueue> _forwardTransparentQueue;

        Vector<RenderableElement*> _instancedElements; //Elements are updated every frame

        static PerInstanceData _instanceDataPool[STANDARD_FORWARD_MAX_INSTANCED_BLOCKS_NUMBER][STANDARD_FORWARD_MAX_INSTANCED_BLOCK_SIZE];
        static Vector<InstancedBuffer> _instancedBuffersPool;

        // Exposure
        float _previousEyeAdaptation = 0.0f;
        float _currentEyeAdaptation = 0.0f;
    };

    /** Contains one or multiple RendererView%s that are in some way related. */
    class RendererViewGroup
    {
    public:
        RendererViewGroup(RendererView** views, UINT32 numViews, SPtr<RenderManOptions> options);

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
         * Returns information about lights visible from this group of views. Only valid after a call to
         * DetermineVisibility().
         */
        const VisibleLightData& GetVisibleLightData() const { return _visibleLightData; }

        /**
         * Updates visibility information for the provided scene objects, from the perspective of all views in this group,
         * and updates the render queues of each individual view. Use getVisibilityInfo() to retrieve the calculated
         * visibility information.
         */
        void DetermineVisibility(const SceneInfo& sceneInfo);

        /**
         * Updates visibility (true) for for the provided scene objects
         */
        void SetAllObjectsAsVisible(const SceneInfo& sceneInfo);

        /**
        * Before creating render queue, we look for all possibly instanced elements
        */
        void GenerateInstanced(const SceneInfo& sceneInfo, RenderManInstancing instancingMode);
    
        /**
        * Once we have set visibility information for all Renderables, we wil decide if some of them can be instanced
        */
        void GenerateRenderQueue(const SceneInfo& sceneInfo, RendererView& view, RenderManInstancing instancingMode);

    private:
        friend class RenderView;

    private:
        SPtr<RenderManOptions> _options;
        Vector<RendererView*> _views;
        VisibilityInfo _visibility;

        VisibleLightData _visibleLightData;
    };

    IMPLEMENT_GLOBAL_POOL(RenderableElement, STANDARD_FORWARD_MAX_INSTANCED_BLOCK_SIZE)
}
