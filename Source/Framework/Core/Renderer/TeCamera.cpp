#include "TeCamera.h"
#include "Renderer/TeViewport.h"
#include "Manager/TeRendererManager.h"
#include "Scene/TeSceneManager.h"
#include "Math/TeRect2I.h"
#include "RenderAPI/TeRenderAPI.h"

namespace te
{
    const float Camera::INFINITE_FAR_PLANE_ADJUST = 0.00001f;

    Camera::Camera(SPtr<RenderTarget> target, float left, float top, float width, float height)
        : _needComputeFrustum(true)
        , _needComputeFrustumPlanes(true)
        , _needComputeView(true)
        , _rendererId(0)
        , _cameraFlags(0)
    {
        InvalidateFrustum();
        _viewport = Viewport::Create(target, left, top, width, height);
        _renderSettings = te_shared_ptr_new<RenderSettings>();
    }

    Camera::Camera(const SPtr<Viewport>& viewport)
        : _needComputeFrustum(true)
        , _needComputeFrustumPlanes(true)
        , _needComputeView(true)
        , _rendererId(0)
        , _cameraFlags(0)
    {
        InvalidateFrustum();
        _viewport = viewport;
        _renderSettings = te_shared_ptr_new<RenderSettings>();
    }

    Camera::~Camera()
    { 
        gRenderer()->NotifyCameraRemoved(this);
    }

    void Camera::Initialize()
    {
        CoreObject::Initialize();
        gSceneManager()._registerCamera(std::static_pointer_cast<Camera>(GetThisPtr()));
        gRenderer()->NotifyCameraAdded(this);
    }

    void Camera::Destroy()
    {
        CoreObject::Destroy();
    }

    void Camera::_markCoreDirty(ActorDirtyFlag flag)
    {
        MarkCoreDirty((UINT32)flag);
        gRenderer()->NotifyCameraUpdated(this, (UINT32)flag);
    }

    void Camera::SetMobility(ObjectMobility mobility)
    {
        SceneActor::SetMobility(mobility);
        _markCoreDirty(ActorDirtyFlag::Mobility);
    }

    /** Returns the viewport used by the camera. */
    void Camera::SetRenderTarget(SPtr<RenderTarget> renderTarget)
    {
        _viewport->SetTarget(renderTarget);
        _markCoreDirty();
    }

    SPtr<Camera> Camera::Create()
    {
        Camera* camera = new (te_allocate<Camera>()) Camera();
        SPtr<Camera> handlerPtr = te_core_ptr<Camera>(camera);
        handlerPtr->SetThisPtr(handlerPtr);
        handlerPtr->Initialize();

        return handlerPtr;
    }

    void Camera::SetTransform(const Transform& transform)
    {
        SceneActor::SetTransform(transform);
        _needComputeView = true;
        _markCoreDirty(ActorDirtyFlag::Transform);
    }

    void Camera::SetFlags(UINT32 flags)
    {
        _cameraFlags = flags;
        _markCoreDirty();
    }

    void Camera::SetHorzFOV(const Radian& fov)
    {
        if (fov <= Radian(Math::PI))
            _horzFOV = fov;
        else
            _horzFOV = Radian(Math::PI);

        InvalidateFrustum();
        _markCoreDirty();
    }

    void Camera::SetNearClipDistance(float nearPlane)
    {
        if (nearPlane <= 0)
        {
            TE_DEBUG("Near clip distance must be greater than zero.", __FILE__, __LINE__);
            return;
        }

        _nearDist = nearPlane;
        InvalidateFrustum();
        _markCoreDirty();
    }

    void Camera::SetFarClipDistance(float farPlane)
    {
        _farDist = farPlane;
        InvalidateFrustum();
        _markCoreDirty();
    }

    void Camera::SetAspectRatio(float r)
    {
        _aspect = r;
        InvalidateFrustum();
        _markCoreDirty();
    }

    float Camera::GetAspectRatio() const
    {
        return _aspect;
    }

    const Matrix4& Camera::GetProjectionMatrixRS() const
    {
        UpdateFrustum();
        return _projMatrixRS;
    }

    const Matrix4& Camera::GetProjectionMatrixRSInv() const
    {
        UpdateFrustum();
        return _projMatrixRSInv;
    }

    const Matrix4& Camera::GetProjectionMatrix() const
    {
        UpdateFrustum();
        return _projMatrix;
    }

    const Matrix4& Camera::GetProjectionMatrixInv() const
    {
        UpdateFrustum();
        return _projMatrixInv;
    }

    const Matrix4& Camera::GetViewMatrix() const
    {
        UpdateView();
        return _viewMatrix;
    }

    const Matrix4& Camera::GetViewMatrixInv() const
    {
        UpdateView();
        return _viewMatrixInv;
    }

    const ConvexVolume& Camera::GetFrustum() const
    {
        // Make any pending updates to the calculated frustum planes
        UpdateFrustumPlanes();
        return _frustum;
    }

    ConvexVolume Camera::GetWorldFrustum() const
    {
        const Vector<Plane>& frustumPlanes = GetFrustum().GetPlanes();

        const Transform& tfrm = GetTransform();

        Matrix4 worldMatrix;
        worldMatrix.SetTRS(tfrm.GetPosition(), tfrm.GetRotation(), Vector3::ONE);

        Vector<Plane> worldPlanes(frustumPlanes.size());
        UINT32 i = 0;
        for (auto& plane : frustumPlanes)
        {
            worldPlanes[i] = worldMatrix.MultiplyAffine(plane);
            i++;
        }

        return ConvexVolume(worldPlanes);
    }

    const AABox& Camera::GetBoundingBox() const
    {
        UpdateFrustum();
        return _boundingBox;
    }

    void Camera::SetProjectionType(ProjectionType pt)
    {
        _projType = pt;
        InvalidateFrustum();
        _markCoreDirty();
    }

    ProjectionType Camera::GetProjectionType() const
    {
        return _projType;
    }

    void Camera::SetOrthoWindow(float w, float h)
    {
        _orthoHeight = h;
        _aspect = w / h;

        InvalidateFrustum();
        _markCoreDirty();
    }

    void Camera::SetOrthoWindowHeight(float h)
    {
        _orthoHeight = h;
        InvalidateFrustum();
        _markCoreDirty();
    }

    float Camera::GetOrthoWindowHeight() const
    {
        return _orthoHeight;
    }

    void Camera::SetOrthoWindowWidth(float w)
    {
        _orthoHeight = w / _aspect;
        InvalidateFrustum();
        _markCoreDirty();
    }

    float Camera::GetOrthoWindowWidth() const
    {
        return _orthoHeight * _aspect;
    }

    Vector2I Camera::WorldToScreenPoint(const Vector3& worldPoint) const
    {
        Vector2 ndcPoint = WorldToNdcPoint(worldPoint);
        return NdcToScreenPoint(ndcPoint);
    }

    Vector2 Camera::WorldToNdcPoint(const Vector3& worldPoint) const
    {
        Vector3 viewPoint = WorldToViewPoint(worldPoint);
        return ViewToNdcPoint(viewPoint);
    }

    Vector3 Camera::WorldToViewPoint(const Vector3& worldPoint) const
    {
        return GetViewMatrix().MultiplyAffine(worldPoint);
    }

    Vector3 Camera::ScreenToWorldPoint(const Vector2I& screenPoint, float depth) const
    {
        Vector2 ndcPoint = ScreenToNdcPoint(screenPoint);
        return NdcToWorldPoint(ndcPoint, depth);
    }

    Vector3 Camera::ScreenToWorldPointDeviceDepth(const Vector2I& screenPoint, float deviceDepth) const
    {
        Vector2 ndcPoint = ScreenToNdcPoint(screenPoint);
        Vector4 worldPoint(ndcPoint.x, ndcPoint.y, deviceDepth, 1.0f);
        worldPoint = GetProjectionMatrix().Inverse().Multiply(worldPoint); //RS

        Vector3 worldPoint3D;
        if (Math::Abs(worldPoint.w) > 1e-7f)
        {
            float invW = 1.0f / worldPoint.w;

            worldPoint3D.x = worldPoint.x * invW;
            worldPoint3D.y = worldPoint.y * invW;
            worldPoint3D.z = worldPoint.z * invW;
        }

        return ViewToWorldPoint(worldPoint3D);
    }

    Vector3 Camera::ScreenToViewPoint(const Vector2I& screenPoint, float depth) const
    {
        Vector2 ndcPoint = ScreenToNdcPoint(screenPoint);
        return NdcToViewPoint(ndcPoint, depth);
    }

    Vector2 Camera::ScreenToNdcPoint(const Vector2I& screenPoint) const
    {
        Rect2I viewport = GetViewportRect();

        Vector2 ndcPoint;
        ndcPoint.x = (float)(((screenPoint.x - viewport.x) / (float)viewport.width) * 2.0f - 1.0f);

        const Conventions& rapiConventions = gCaps().Convention;
        if (rapiConventions.NDC_YAxis == Conventions::Axis::Down)
        {
            ndcPoint.y = (float)(((screenPoint.y - viewport.y) / (float)viewport.height) * 2.0f - 1.0f);
        }
        else
        {
            ndcPoint.y = (float)((1.0f - ((screenPoint.y - viewport.y) / (float)viewport.height)) * 2.0f - 1.0f);
        }

        return ndcPoint;
    }

    Vector3 Camera::ViewToWorldPoint(const Vector3& viewPoint) const
    {
        return GetViewMatrix().InverseAffine().MultiplyAffine(viewPoint);
    }

    Vector2I Camera::ViewToScreenPoint(const Vector3& viewPoint) const
    {
        Vector2 ndcPoint = ViewToNdcPoint(viewPoint);
        return NdcToScreenPoint(ndcPoint);
    }

    Vector2 Camera::ViewToNdcPoint(const Vector3& viewPoint) const
    {
        Vector3 projPoint = ProjectPoint(viewPoint);
        return Vector2(projPoint.x, projPoint.y);
    }

    Vector3 Camera::NdcToWorldPoint(const Vector2& ndcPoint, float depth) const
    {
        Vector3 viewPoint = NdcToViewPoint(ndcPoint, depth);
        return ViewToWorldPoint(viewPoint);
    }

    Vector3 Camera::NdcToViewPoint(const Vector2& ndcPoint, float depth) const
    {
        return UnprojectPoint(Vector3(ndcPoint.x, ndcPoint.y, depth));
    }

    Vector2I Camera::NdcToScreenPoint(const Vector2& ndcPoint) const
    {
        Rect2I viewport = GetViewportRect();

        Vector2I screenPoint;
        screenPoint.x = Math::RoundToInt(viewport.x + ((ndcPoint.x + 1.0f) * 0.5f) * viewport.width);

        const Conventions& rapiConventions = gCaps().Convention;
        if (rapiConventions.NDC_YAxis == Conventions::Axis::Down)
        {
            screenPoint.y = Math::RoundToInt(viewport.y + (ndcPoint.y + 1.0f) * 0.5f * viewport.height);
        }
        else
        {
            screenPoint.y = Math::RoundToInt(viewport.y + (1.0f - (ndcPoint.y + 1.0f) * 0.5f) * viewport.height);
        }

        return screenPoint;
    }

    Ray Camera::ScreenPointToRay(const Vector2I& screenPoint) const
    {
        Vector2 ndcPoint = ScreenToNdcPoint(screenPoint);

        Vector3 near = UnprojectPoint(Vector3(ndcPoint.x, ndcPoint.y, _nearDist));
        Vector3 far = UnprojectPoint(Vector3(ndcPoint.x, ndcPoint.y, _nearDist + 1.0f));

        Ray ray(near, Vector3::Normalize(far - near));
        ray.TransformAffine(GetViewMatrix().InverseAffine());

        return ray;
    }

    Vector3 Camera::ProjectPoint(const Vector3& point) const
    {
        Vector4 projPoint4(point.x, point.y, point.z, 1.0f);
        projPoint4 = GetProjectionMatrix().Multiply(projPoint4); //RS

        if (Math::Abs(projPoint4.w) > 1e-7f)
        {
            float invW = 1.0f / projPoint4.w;
            projPoint4.x *= invW;
            projPoint4.y *= invW;
            projPoint4.z *= invW;
        }
        else
        {
            projPoint4.x = 0.0f;
            projPoint4.y = 0.0f;
            projPoint4.z = 0.0f;
        }

        return Vector3(projPoint4.x, projPoint4.y, projPoint4.z);
    }

    Vector3 Camera::UnprojectPoint(const Vector3& point) const
    {
        // Point.z is expected to be in view space, so we need to do some extra work to get the proper coordinates
        // (as opposed to if point.z was in device coordinates, in which case we could just inverse project)

        // Get world position for a point near the far plane (0.95f)
        Vector4 farAwayPoint(point.x, point.y, 0.95f, 1.0f);
        farAwayPoint = GetProjectionMatrix().Inverse().Multiply(farAwayPoint); //RS

        // Can't proceed if w is too small
        if (Math::Abs(farAwayPoint.w) > 1e-7f)
        {
            // Perspective divide, to get the values that make sense in 3D space
            float invW = 1.0f / farAwayPoint.w;

            Vector3 farAwayPoint3D;
            farAwayPoint3D.x = farAwayPoint.x * invW;
            farAwayPoint3D.y = farAwayPoint.y * invW;
            farAwayPoint3D.z = farAwayPoint.z * invW;

            // Find the distance to the far point along the camera's viewing axis
            float distAlongZ = farAwayPoint3D.Dot(-Vector3::UNIT_Z);

            // Do nothing if point is behind the camera
            if (distAlongZ >= 0.0f)
            {
                if (_projType == PT_PERSPECTIVE)
                {
                    // Direction from origin to our point
                    Vector3 dir = farAwayPoint3D; // Camera is at (0, 0, 0) so it's the same vector

                    // Our view space depth (point.z) is distance along the camera's viewing axis. Since our direction
                    // vector is not parallel to the viewing axis, instead of normalizing it with its own length, we
                    // "normalize" with the length projected along the camera's viewing axis.
                    dir /= distAlongZ;

                    // And now we just find the final position along the direction
                    return dir * point.z;
                }
                else // Ortographic
                {
                    // Depth difference between our arbitrary point and actual depth
                    float depthDiff = distAlongZ - point.z;

                    // Depth difference along viewing direction
                    Vector3 depthDiffVec = depthDiff * -Vector3::UNIT_Z;

                    // Return point that is depthDiff closer than our arbitrary point
                    return farAwayPoint3D - depthDiffVec;
                }
            }
        }

        return Vector3(0.0f, 0.0f, 0.0f);
    }

    void Camera::ComputeProjectionParameters(float& left, float& right, float& bottom, float& top) const
    {

        if (_projType == PT_PERSPECTIVE)
        {
            Radian thetaX(_horzFOV * 0.5f);
            float tanThetaX = Math::Tan(thetaX);
            float tanThetaY = tanThetaX / _aspect;

            float half_w = tanThetaX * _nearDist;
            float half_h = tanThetaY * _nearDist;

            left = -half_w;
            right = half_w;
            bottom = -half_h;
            top = half_h;

            _left = left;
            _right = right;
            _top = top;
            _bottom = bottom;
        }
        else
        {
            float half_w = GetOrthoWindowWidth() * 0.5f;
            float half_h = GetOrthoWindowHeight() * 0.5f;

            left = -half_w;
            right = half_w;
            bottom = -half_h;
            top = half_h;

            _left = left;
            _right = right;
            _top = top;
            _bottom = bottom;
        }
    }

    void Camera::UpdateFrustum() const
    {
        if (IsFrustumOutOfDate())
        {
            float left, right, bottom, top;

            ComputeProjectionParameters(left, right, bottom, top);

            float inv_w = 1 / (right - left);
            float inv_h = 1 / (top - bottom);
            float inv_d = 1 / (_farDist - _nearDist);

            if (_projType == PT_PERSPECTIVE)
            {
                float A = 2 * _nearDist * inv_w;
                float B = 2 * _nearDist * inv_h;
                float C = (right + left) * inv_w;
                float D = (top + bottom) * inv_h;
                float q, qn;

                if (_farDist == 0)
                {
                    // Infinite far plane
                    q = Camera::INFINITE_FAR_PLANE_ADJUST - 1;
                    qn = _nearDist * (Camera::INFINITE_FAR_PLANE_ADJUST - 2);
                }
                else
                {
                    q = -(_farDist + _nearDist) * inv_d;
                    qn = -2 * (_farDist * _nearDist) * inv_d;
                }

                _projMatrix = Matrix4::ZERO;
                _projMatrix[0][0] = A;
                _projMatrix[0][2] = C;
                _projMatrix[1][1] = B;
                _projMatrix[1][2] = D;
                _projMatrix[2][2] = q;
                _projMatrix[2][3] = qn;
                _projMatrix[3][2] = -1;
            }
            else if (_projType == PT_ORTHOGRAPHIC)
            {
                float A = 2 * inv_w;
                float B = 2 * inv_h;
                float C = -(right + left) * inv_w;
                float D = -(top + bottom) * inv_h;
                float q, qn;

                if (_farDist == 0)
                {
                    // Can not do infinite far plane here, avoid divided zero only
                    q = -Camera::INFINITE_FAR_PLANE_ADJUST / _nearDist;
                    qn = -Camera::INFINITE_FAR_PLANE_ADJUST - 1;
                }
                else
                {
                    q = -2 * inv_d;
                    qn = -(_farDist + _nearDist) * inv_d;
                }

                _projMatrix = Matrix4::ZERO;
                _projMatrix[0][0] = A;
                _projMatrix[0][3] = C;
                _projMatrix[1][1] = B;
                _projMatrix[1][3] = D;
                _projMatrix[2][2] = q;
                _projMatrix[2][3] = qn;
                _projMatrix[3][3] = 1;
            }

            RenderAPI* renderAPI = RenderAPI::InstancePtr();
            renderAPI->ConvertProjectionMatrix(_projMatrix, _projMatrixRS);
            _projMatrixInv = _projMatrix.Inverse();
            _projMatrixRSInv = _projMatrixRS.Inverse();

            // Calculate bounding box (local)
            // Box is from 0, down -Z, max dimensions as determined from far plane
            // If infinite view frustum just pick a far value
            float farDist = (_farDist == 0) ? 100000 : _farDist;

            // Near plane bounds
            Vector3 min(left, bottom, -farDist);
            Vector3 max(right, top, 0);

            if (_projType == PT_PERSPECTIVE)
            {
                // Merge with far plane bounds
                float radio = farDist / _nearDist;
                min.Min(Vector3(left * radio, bottom * radio, -farDist));
                max.Max(Vector3(right * radio, top * radio, 0));
            }

            _boundingBox.SetExtents(min, max);

            _needComputeFrustum = false;
            _needComputeFrustumPlanes = true;
        }
    }

    void Camera::UpdateFrustumPlanes() const
    {
        UpdateFrustum();

        if (_needComputeFrustumPlanes)
        {
            _frustum = ConvexVolume(_projMatrix);
            _needComputeFrustumPlanes = false;
        }
    }

    void Camera::UpdateView() const
    {
        if (_needComputeView)
        {
            _viewMatrix.MakeView(_transform.GetPosition(), _transform.GetRotation());
            _viewMatrixInv = _viewMatrix.InverseAffine();
            _needComputeView = false;
        }
    }

    bool Camera::IsFrustumOutOfDate() const
    {
        return _needComputeFrustum;
    }

    void Camera::InvalidateFrustum() const
    {
        _needComputeFrustum = true;
        _needComputeFrustumPlanes = true;
    }

    Rect2I Camera::GetViewportRect() const
    {
        return _viewport->GetPixelArea();
    }
}
