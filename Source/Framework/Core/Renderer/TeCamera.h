#pragma once

#include "TeCorePrerequisites.h"
#include "Scene/TeSceneActor.h"
#include "Renderer/TeViewport.h"
#include "Renderer/TeRenderSettings.h"
#include "CoreUtility/TeCoreObject.h"
#include "Math/TeMatrix4.h"
#include "Math/TeVector3.h"
#include "Math/TeVector2.h"
#include "Math/TeVector2I.h"
#include "Math/TeAABox.h"
#include "Math/TeQuaternion.h"
#include "Math/TeRay.h"
#include "Math/TeConvexVolume.h"
#include "Serialization/TeSerializable.h"

namespace te
{
    /** Flags for controlling Camera options. */
    enum class CameraFlag
    {
        /**
         * If set the camera will only render when requested by the user through Camera::NotifyNeedsRedraw().
         * Otherwise the camera will render every frame (unless disabled).
         */
        OnDemand = 1 << 0,
    };

    /**	Signals which portion of a Camera is dirty. */
    enum class CameraDirtyFlag
    {
        // First few bits reserved by ActorDirtyFlag
        RenderSettings = 1 << 5,
        Redraw = 1 << 6,
        Viewport = 1 << 31
    };

    /**
     * Camera determines how is world geometry projected onto a 2D surface. You may position and orient it in space, set
     * options like aspect ratio and field or view and it outputs view and projection matrices required for rendering.
     */
    class TE_CORE_EXPORT Camera : public CoreObject, public SceneActor, public Serializable
    {
    public:
        virtual ~Camera();

        /** @copydoc CoreObject::Destroy */
        void Destroy() override;

        /** @copydoc SceneActor::Destroy */
        void SetMobility(ObjectMobility mobility) override;

        /** @copydoc SceneActor::SetTransform */
        void SetTransform(const Transform& transform) override;

        /**	Sets an ID that can be used for uniquely identifying this object by the renderer. */
        void SetRendererId(UINT32 id) { _rendererId = id; }

        /**	Retrieves an ID that can be used for uniquely identifying this object by the renderer. */
        UINT32 GetRendererId() const { return _rendererId; }

        /**
         * Determines whether this is the main application camera. Main camera controls the final render surface that is
         * displayed to the user.
         */
        void SetMain(bool main) { _main = main; };

        /** @copydoc SetMain */
        bool IsMain() const { return _main; }

        /** Returns the viewport used by the camera. */
        const SPtr<Viewport> GetViewport() { return _viewport; }

        /**	Returns a rectangle that defines the viewport position and size, in pixels. */
        virtual Rect2I GetViewportRect() const;

        /** */
        void SetRenderTarget(SPtr<RenderTarget> renderTarget);

        /** Determines flags used for controlling the camera behaviour. */
        void SetFlags(UINT32 flag);

        /** @copydoc SetFlags */
        UINT32 GetFlags() const { return _cameraFlags; }

        /** @copydoc SetHorzFOV */
        virtual const Radian& GetHorzFOV() const { return _horzFOV; }

        /**	Determines the current viewport aspect ratio (width / height). */
        virtual void SetAspectRatio(float ratio);

        /** @copydoc SetAspectRatio */
        virtual float GetAspectRatio() const { return _aspect; }

        /**	Determines the focal length which equivalent to FOV. Both can be computed using the other. Expressed in mm */
        virtual void SetFocalLength(float focalLength);

        /** @copydoc SetFocalLength */
        virtual float GetFocalLength() const { return _focalLength; }

        /**	Determines how much light hit the sensor. Expressed using f-stop */
        virtual void SetAperture(float aperture);

        /** @copydoc SetAperture */
        virtual float GetAperture() const { return _aperture; }

        /**	Determines how long the sensor is exposed. Expressed in second */
        virtual void SetShutterSpeed(float shutterSpeed);

        /** @copydoc SetShutterSpeed */
        virtual float GetShutterSpeed() const { return _shutterSpeed; }

        /**	Determines the sensitivity of the sensor. Expressed using ISO notation */
        virtual void SetSensitivity(UINT32 sensitivity);

        /** @copydoc SetSensitivity */
        virtual UINT32 GetSensitivity() const { return _sensitivity; }

        /** Get Ev100 */
        virtual float GetEv100() const { return _ev100; }

        /** Get Exposure */
        virtual float GetExposure() const { return _exposure; }

        /**
         * Determines the distance from the frustum to the near clipping plane. Anything closer than the near clipping plane will
         * not be rendered. Decreasing this value decreases depth buffer precision.
         */
        virtual void SetNearClipDistance(float nearDist);

        /** @copydoc setNearClipDistance */
        virtual float GetNearClipDistance() const { return _nearDist; }

        /**
         * Determines the distance from the frustum to the far clipping plane. Anything farther than the far clipping plane will
         * not be rendered. Increasing this value decreases depth buffer precision.
         */
        virtual void SetFarClipDistance(float farDist);

        /** @copydoc SetFarClipDistance */
        virtual float GetFarClipDistance() const { return _farDist; }

        /** Determines the type of projection used by the camera. Projection type controls how is 3D geometry projected onto a 2D plane. */
        virtual void SetProjectionType(ProjectionType pt);

        /** @copydoc SetProjectionType */
        virtual ProjectionType GetProjectionType() const;

        /**
         * Returns the standard projection matrix that determines how are 3D points projected to two dimensions. The layout
         * of this matrix depends on currently used render system.
         *
         * @note
         * You should use this matrix when sending the matrix to the render system to make sure everything works
         * consistently when other render systems are used.
         */
        virtual const Matrix4& GetProjectionMatrixRS() const;

        /** Returns the inverse of the render-system specific projection matrix. See getProjectionMatrixRS(). */
        virtual const Matrix4& GetProjectionMatrixRSInv() const;

        /**
         * Returns the standard projection matrix that determines how are 3D points projected to two dimensions. Returned
         * matrix is standard following right-hand rules and depth range of [-1, 1]. In case you need a render-system specific
         * projection matrix call GetProjectionMatrixRS().
         */
        virtual const Matrix4& GetProjectionMatrix() const;

        /** Returns the inverse of the projection matrix. See getProjectionMatrix(). */
        virtual const Matrix4& GetProjectionMatrixInv() const;

        /** Gets the camera view matrix. Used for positioning/orienting the camera. */
        virtual const Matrix4& GetViewMatrix() const;

        /** Returns the inverse of the view matrix. See getViewMatrix(). */
        virtual const Matrix4& GetViewMatrixInv() const;

        /** Returns a convex volume representing the visible area of the camera, in local space. */
        virtual const ConvexVolume& GetFrustum() const;

        /** Returns a convex volume representing the visible area of the camera, in world space. */
        virtual ConvexVolume GetWorldFrustum() const;

        /**	Returns the bounding of the frustum. */
        const AABox& GetBoundingBox() const;

        /**
         * Sets the orthographic window height, for use with orthographic rendering only.
         *
         * @param[in]	w	Width of the window in world units.
         * @param[in]	h	Height of the window in world units.
         *
         * @note
         * Calling this method will recalculate the aspect ratio, use setOrthoWindowHeight() or setOrthoWindowWidth() alone
         * if you wish to preserve the aspect ratio but just fit one or other dimension to a particular size.
         */
        virtual void SetOrthoWindow(float w, float h);

        /**
         * Determines the orthographic window height, for use with orthographic rendering only. The width of the window
         * will be calculated from the aspect ratio. Value is specified in world units.
         */
        virtual void SetOrthoWindowHeight(float h);

        /** @copydoc setOrthoWindowHeight */
        virtual float GetOrthoWindowHeight() const;

        /**
         * Determines the orthographic window width, for use with orthographic rendering only. The height of the window
         * will be calculated from the aspect ratio. Value is specified in world units.
         */
        virtual void SetOrthoWindowWidth(float w);

        /** @copydoc setOrthoWindowWidth */
        virtual float GetOrthoWindowWidth() const;

        /**
         * Determines number of samples to use when rendering to this camera. Values larger than 1 will enable MSAA
         * rendering.
         */
        void SetMSAACount(UINT32 count) { _MSAA = count; }

        /** @copydoc SetMSAACount */
        UINT32 GetMSAACount() const { return _MSAA; }

        /**
         * Determines a priority that determines in which orders the cameras are rendered. This only applies to cameras rendering
         * to the same render target. Higher value means the camera will be rendered sooner.
         */
        void SetPriority(INT32 priority) { _priority = priority; _markCoreDirty();  }

        /** @copydoc SetPriority */
        INT32 GetPriority() const { return _priority; }

        /**	Determines layer bitfield that is used when determining which object should the camera render. */
        void SetLayers(UINT64 layers) { _layers = layers; _markCoreDirty(); }

        /** @copydoc SetLayers */
        UINT64 GetLayers() const { return _layers; }

        /**
         * Settings that control rendering for this view. They determine how will the renderer process this view, which
         * effects will be enabled, and what properties will those effects use.
         */
        void SetRenderSettings(const SPtr<RenderSettings>& settings)
        {
            _renderSettings = settings;
            _markCoreDirty((ActorDirtyFlag)CameraDirtyFlag::RenderSettings);
        }

        /**
         * @copydoc Camera::SetRenderSettings
         * @note : this method will use copy constructor instead of just changing shared pointer value
         */
        void SetRenderSettings(const RenderSettings& settings)
        {
            (*_renderSettings) = settings;
            _markCoreDirty((ActorDirtyFlag)CameraDirtyFlag::RenderSettings);
        }

        /** @copydoc SetRenderSettings */
        const SPtr<RenderSettings>& GetRenderSettings() const { return _renderSettings; }

        /**
         * Notifies a on-demand camera that it should re-draw its contents on the next frame. Ignored for a camera
         * that isn't on-demand.
         */
        void NotifyNeedsRedraw()
        {
            if (GetFlags() & (UINT32)CameraFlag::OnDemand)
                _markCoreDirty((ActorDirtyFlag)CameraDirtyFlag::Redraw);
        }

        /**
         * Notifies a on-demand camera that it should re-draw its contents on the next frame. Ignored for a camera
         * that isn't on-demand.
         */
        void NotifyUpdateEverything()
        {
            _markCoreDirty();
        }

        /**
         * You can change at runtime which renderer will handle this camera
         * Current renderer will be notified that camera must be removed
         * And next renderer will be notified that camera must be added
         */
        void AttachTo(SPtr<Renderer> renderer = nullptr);

        /** Creates a new camera that renders to the specified portion of the provided render target. */
        static SPtr<Camera> Create();

    public:
        /**
         * Converts a point in world space to screen coordinates.
         *
         * @param[in]	worldPoint		3D point in world space.
         * @return						2D point on the render target attached to the camera's viewport, in pixels.
         */
        Vector2I WorldToScreenPoint(const Vector3& worldPoint) const;

        /**
         * Converts a point in world space to normalized device coordinates.
         *
         * @param[in]	worldPoint		3D point in world space.
         * @return						2D point in normalized device coordinates ([-1, 1] range), relative to the camera's viewport.
         */
        Vector2 WorldToNdcPoint(const Vector3& worldPoint) const;

        /**
         * Converts a point in world space to view space coordinates.
         *
         * @param[in]	worldPoint		3D point in world space.
         * @return						3D point relative to the camera's coordinate system.
         */
        Vector3 WorldToViewPoint(const Vector3& worldPoint) const;

        /**
         * Converts a point in screen space to a point in world space.
         *
         * @param[in]	screenPoint	2D point on the render target attached to the camera's viewport, in pixels.
         * @param[in]	depth		Depth to place the world point at, in world coordinates. The depth is applied to the
         *							vector going from camera origin to the point on the near plane.
         * @return					3D point in world space.
         */
        Vector3 ScreenToWorldPoint(const Vector2I& screenPoint, float depth = 0.5f) const;

        /**
        * Converts a point in screen space (pixels corresponding to render target attached to the camera) to a point in
        * world space.
        *
        * @param[in]	screenPoint	Point to transform.
        * @param[in]	deviceDepth	Depth to place the world point at, in normalized device coordinates.
        * @return					3D point in world space.
        */
        Vector3 ScreenToWorldPointDeviceDepth(const Vector2I& screenPoint, float deviceDepth = 0.5f) const;

        /**
         * Converts a point in screen space to a point in view space.
         *
         * @param[in]	screenPoint	2D point on the render target attached to the camera's viewport, in pixels.
         * @param[in]	depth		Depth to place the world point at, in device depth. The depth is applied to the
         *							vector going from camera origin to the point on the near plane.
         * @return					3D point relative to the camera's coordinate system.
         */
        Vector3 ScreenToViewPoint(const Vector2I& screenPoint, float depth = 0.5f) const;

        /**
         * Converts a point in screen space to normalized device coordinates.
         *
         * @param[in]	screenPoint		2D point on the render target attached to the camera's viewport, in pixels.
         * @return						2D point in normalized device coordinates ([-1, 1] range), relative to
         *								the camera's viewport.
         */
        Vector2 ScreenToNdcPoint(const Vector2I& screenPoint) const;

        /**
         * Converts a point in view space to world space.
         *
         * @param[in]	viewPoint		3D point relative to the camera's coordinate system.
         * @return						3D point in world space.
         */
        Vector3 ViewToWorldPoint(const Vector3& viewPoint) const;

        /**
         * Converts a point in view space to screen space.
         *
         * @param[in]	viewPoint		3D point relative to the camera's coordinate system.
         * @return						2D point on the render target attached to the camera's viewport, in pixels.
         */
        Vector2I ViewToScreenPoint(const Vector3& viewPoint) const;

        /**
         * Converts a point in view space to normalized device coordinates.
         *
         * @param[in]	viewPoint		3D point relative to the camera's coordinate system.
         * @return						2D point in normalized device coordinates ([-1, 1] range), relative to
         *								the camera's viewport.
         */
        Vector2 ViewToNdcPoint(const Vector3& viewPoint) const;

        /**
         * Converts a point in normalized device coordinates to world space.
         *
         * @param[in]	ndcPoint	2D point in normalized device coordinates ([-1, 1] range), relative to
         *							the camera's viewport.
         * @param[in]	depth		Depth to place the world point at. The depth is applied to the
         *							vector going from camera origin to the point on the near plane.
         * @return					3D point in world space.
         */
        Vector3 NdcToWorldPoint(const Vector2& ndcPoint, float depth = 0.5f) const;

        /**
         * Converts a point in normalized device coordinates to view space.
         *
         * @param[in]	ndcPoint	2D point in normalized device coordinates ([-1, 1] range), relative to
         *							the camera's viewport.
         * @param[in]	depth		Depth to place the world point at. The depth is applied to the
         *							vector going from camera origin to the point on the near plane.
         * @return					3D point relative to the camera's coordinate system.
         */
        Vector3 NdcToViewPoint(const Vector2& ndcPoint, float depth = 0.5f) const;

        /**
         * Converts a point in normalized device coordinates to screen space.
         *
         * @param[in]	ndcPoint	2D point in normalized device coordinates ([-1, 1] range), relative to
         *							the camera's viewport.
         * @return					2D point on the render target attached to the camera's viewport, in pixels.
         */
        Vector2I NdcToScreenPoint(const Vector2& ndcPoint) const;

        /**
         * Converts a point in screen space to a ray in world space.
         *
         * @param[in]	screenPoint		2D point on the render target attached to the camera's viewport, in pixels.
         * @return						Ray in world space, originating at the selected point on the camera near plane.
         */
        Ray ScreenPointToRay(const Vector2I& screenPoint) const;

        /**
         * Projects a point in view space to normalized device coordinates. Similar to viewToNdcPoint() but preserves
         * the depth component.
         *
         * @param[in]	point			3D point relative to the camera's coordinate system.
         * @return						3D point in normalized device coordinates ([-1, 1] range), relative to the
         *								camera's viewport. Z value range depends on active render API.
         */
        Vector3 ProjectPoint(const Vector3& point) const;

        /**	Un-projects a point in normalized device space to view space.
         *
         * @param[in]	point			3D point in normalized device coordinates ([-1, 1] range), relative to the
         *								camera's viewport. Z value range depends on active render API.
         * @return						3D point relative to the camera's coordinate system.
         */
        Vector3 UnprojectPoint(const Vector3& point) const;

    public:
        static const float INFINITE_FAR_PLANE_ADJUST; /**< Small constant used to reduce far plane projection to avoid inaccuracies. */

        // a 35mm camera has a 36x24mm wide frame size
        static const float SENSOR_HEIGHT;  // 24mm

    protected:
        /**
         * Determines the camera horizontal field of view. This determines how wide the camera viewing angle is along the
         * horizontal axis. Vertical FOV is calculated from the horizontal FOV and the aspect ratio.
         */
        virtual void SetHorzFOV(const Radian& fov);

        /** Convert focal length in mm to FOV in radian */
        Radian FocalLengthToFOV(float focalLength);

        /**	Calculate projection parameters that are used when constructing the projection matrix. */
        virtual void ComputeProjectionParameters(float& left, float& right, float& bottom, float& top) const;

        /**	Recalculate frustum if dirty. */
        virtual void UpdateFrustum() const;

        /**	Recalculate frustum planes if dirty. */
        virtual void UpdateFrustumPlanes() const;

        /**
         * Update view matrix from parent position/orientation.
         *
         * @note	Does nothing when custom view matrix is set.
         */
        virtual void UpdateView() const;

        /**	Checks if the frustum requires updating. */
        virtual bool IsFrustumOutOfDate() const;

        /**	Notify camera that the frustum requires to be updated. */
        virtual void InvalidateFrustum() const;

        /** @copydoc SceneActor::_markCoreDirty */
        void _markCoreDirty(ActorDirtyFlag flag = ActorDirtyFlag::Everything) override;

        /** @copydoc CoreObject::FrameSync */
        void FrameSync() override;

    protected:
        friend class CCamera;

        Camera(const SPtr<Viewport>& viewport);
        Camera(SPtr<RenderTarget> target = nullptr, float left = 0.0f, float top = 0.0f, float width = 1.0f, float height = 1.0f);

        /** @copydoc CoreObject::Initialize */
        void Initialize() override;

    protected:
        UINT32 _rendererId = 0;
        UINT64 _layers = 0x1; /**< Bitfield that can be used for filtering what objects the camera sees. */

        ProjectionType _projType = PT_PERSPECTIVE; /**< Type of camera projection. */
        Radian _horzFOV = Degree(90.0f); /**< Horizontal field of view represents how wide is the camera angle. */
        float _focalLength = 24.0f; /**< Focal length representing how wide is the camera angle. Expressed in mm */
        float _aspect = 1.5f; /**< Width/height viewport ratio. */
        float _aperture = 8.f; /**< Expressed in f-stop */
        float _shutterSpeed = 1 / 60.f; /**< Expressed in seconds */
        UINT32 _sensitivity = 100; /**< Expressed using ISO */
        float _ev100 = 1.f;
        float _exposure = 1.f;

        float _farDist = 500.0f; /**< Clip any objects further than this. Larger value decreases depth precision at smaller depths. */
        float _nearDist = 0.1f; /**< Clip any objects close than this. Smaller value decreases depth precision at larger depths. */
        float _orthoHeight = 5; /**< Height in world units used for orthographic cameras. */
        INT32 _priority = 0; /**< Determines in what order will the camera be rendered. Higher priority means the camera will be rendered sooner. */
        bool _main = false; /**< Determines does this camera render to the main render surface. */
        UINT32 _cameraFlags = 0; /**< Flags for controlling various behaviour. */

        SPtr<Viewport> _viewport; /** Viewport that describes a 2D rendering surface. */

        SPtr<RenderSettings> _renderSettings; /** Settings used to control rendering for this camera. */

        UINT32 _MSAA = 1; /**< Number of samples to render the scene with. */

        mutable Matrix4 _projMatrixRS = TeZero; /**< Cached render-system specific projection matrix. */
        mutable Matrix4 _projMatrix = TeZero; /**< Cached projection matrix that determines how are 3D points projected to a 2D viewport. */
        mutable Matrix4 _viewMatrix = TeZero; /**< Cached view matrix that determines camera position/orientation. */
        mutable Matrix4 _projMatrixRSInv = TeZero;
        mutable Matrix4 _projMatrixInv = TeZero;
        mutable Matrix4 _viewMatrixInv = TeZero;

        mutable ConvexVolume _frustum; /**< Main clipping planes describing cameras visible area. */
        mutable bool _needComputeFrustum : 1; /**< Should frustum be recalculated. */
        mutable bool _needComputeFrustumPlanes : 1; /**< Should frustum planes be recalculated. */
        mutable bool _needComputeView : 1; /**< Should view matrix be recalculated. */
        mutable float _left = 0.0f, _right = 0.0f, _top = 0.0f, _bottom = 0.0f; /**< Frustum extents. */
        mutable AABox _boundingBox; /**< Frustum bounding box. */

        SPtr<Renderer> _renderer; /** Default renderer if this attributes is not filled in constructor. */
    };
}
