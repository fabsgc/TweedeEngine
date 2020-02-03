#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeCamera.h"
#include "Scene/TeComponent.h"

namespace te
{
    /**
     * @copydoc	Camera
     *
     * @note	Wraps Camera as a Component.
     */
    class TE_CORE_EXPORT CCamera : public Component
    {
    public:
        /** @copydoc Component::Initialize */
        void Initialize() override;

        /** Return Component type */
        static UINT32 GetComponentType() { return TID_CCamera; }

        /** @copydoc Camera::SetFlags */
        void SetFlags(UINT32 flags) { _internal->SetFlags(flags); }

        /** @copydoc Camera::GetFlags */
        UINT32 GetFlags() const { return _internal->GetFlags(); }

        /** @copydoc Camera::GetViewport */
        SPtr<Viewport> GetViewport() const { return _internal->GetViewport(); }

        /** @copydoc Camera::SetHorzFOV */
        virtual void SetHorzFOV(const Radian& fovy) { _internal->SetHorzFOV(fovy); }

        /** @copydoc Camera::GetHorzFOV */
        virtual const Radian& GetHorzFOV() const { return _internal->GetHorzFOV(); }

        /** @copydoc Camera::SetNearClipDistance */
        virtual void SetNearClipDistance(float nearDist) { _internal->SetNearClipDistance(nearDist); }

        /** @copydoc Camera::GetNearClipDistance */
        virtual float GetNearClipDistance() const { return _internal->GetNearClipDistance(); }

        /** @copydoc Camera::SetFarClipDistance */
        virtual void SetFarClipDistance(float farDist) { _internal->SetFarClipDistance(farDist); }

        /** @copydoc Camera::GetFarClipDistance */
        virtual float GetFarClipDistance() const { return _internal->GetFarClipDistance(); }

        /** @copydoc Camera::SetAspectRatio */
        virtual void SetAspectRatio(float ratio) { _internal->SetAspectRatio(ratio); }

        /** @copydoc Camera::GetAspectRatio */
        virtual float GetAspectRatio() const { return _internal->GetAspectRatio(); }

        /** @copydoc Camera::GetProjectionMatrixRS */
        virtual const Matrix4& GetProjectionMatrixRS() const { return _internal->GetProjectionMatrixRS(); }

        /** @copydoc Camera::GetProjectionMatrix */
        virtual const Matrix4& GetProjectionMatrix() const { return _internal->GetProjectionMatrix(); }

        /** @copydoc Camera::GetViewMatrix */
        virtual const Matrix4& GetViewMatrix() const { UpdateView(); return _internal->GetViewMatrix(); }

        /** @copydoc Camera::GetFrustum */
        virtual const ConvexVolume& GetFrustum() const { return _internal->GetFrustum(); }

        /** @copydoc Camera::GetWorldFrustum */
        virtual ConvexVolume GetWorldFrustum() const;

        /** @copydoc Camera::GetBoundingBox */
        const AABox& GetBoundingBox() const { return _internal->GetBoundingBox(); }

        /** @copydoc Camera::SetProjectionType */
        virtual void SetProjectionType(ProjectionType pt) { _internal->SetProjectionType(pt); }

        /** @copydoc Camera::GetProjectionType */
        virtual ProjectionType GetProjectionType() const { return _internal->GetProjectionType(); }

        /** @copydoc Camera::SetOrthoWindow */
        virtual void SetOrthoWindow(float w, float h) { _internal->SetOrthoWindow(w, h); }

        /** @copydoc Camera::SetOrthoWindowHeight */
        virtual void SetOrthoWindowHeight(float h) { _internal->SetOrthoWindowHeight(h); }

        /** @copydoc Camera::GetOrthoWindowHeight */
        virtual float GetOrthoWindowHeight() const { return _internal->GetOrthoWindowHeight(); }

        /** @copydoc Camera::SetOrthoWindowWidth */
        virtual void SetOrthoWindowWidth(float w) { _internal->SetOrthoWindowWidth(w); }

        /** @copydoc Camera::GetOrthoWindowWidth */
        virtual float GetOrthoWindowWidth() const { return _internal->GetOrthoWindowWidth(); }

        /** @copydoc Camera::SetPriority */
        void SetPriority(INT32 priority) { _internal->SetPriority(priority); }

        /** @copydoc Camera::GetPriority */
        INT32 GetPriority() const { return _internal->GetPriority(); }

        /** @copydoc Camera::SetLayers */
        void SetLayers(UINT64 layers) { _internal->SetLayers(layers); }

        /** @copydoc Camera::GetLayers */
        UINT64 GetLayers() const { return _internal->GetLayers(); }

        /** @copydoc Camera::SetMSAACount */
        void SetMSAACount(UINT32 count) { _internal->SetMSAACount(count); }

        /** @copydoc Camera::GetMSAACount */
        UINT32 GetMSAACount() const { return _internal->GetMSAACount(); }

        /** @copydoc Camera::SetRenderSettings() */
        void SetRenderSettings(const SPtr<RenderSettings>& settings) { _internal->SetRenderSettings(settings); }

        /** @copydoc Camera::GetRenderSettings() */
        const SPtr<RenderSettings>& GetRenderSettings() const { return _internal->GetRenderSettings(); }

        /** @copydoc Camera::NotifyNeedsRedraw() */
        void NotifyNeedsRedraw() { _internal->NotifyNeedsRedraw(); }

        /** @copydoc Camera::worldToScreenPoint */
        Vector2I worldToScreenPoint(const Vector3& worldPoint) const { UpdateView(); return _internal->WorldToScreenPoint(worldPoint); }

        /** @copydoc Camera::worldToNdcPoint */

        Vector2 worldToNdcPoint(const Vector3& worldPoint) const { UpdateView(); return _internal->WorldToNdcPoint(worldPoint); }

        /** @copydoc Camera::worldToViewPoint */
        Vector3 worldToViewPoint(const Vector3& worldPoint) const { UpdateView(); return _internal->WorldToViewPoint(worldPoint); }

        /** @copydoc Camera::screenToWorldPoint */
        Vector3 screenToWorldPoint(const Vector2I& screenPoint, float depth = 0.5f) const { UpdateView(); return _internal->ScreenToWorldPoint(screenPoint, depth); }

        /** @copydoc Camera::screenToViewPoint */
        Vector3 screenToViewPoint(const Vector2I& screenPoint, float depth = 0.5f) const { return _internal->ScreenToViewPoint(screenPoint, depth); }

        /** @copydoc Camera::screenToNdcPoint */
        Vector2 screenToNdcPoint(const Vector2I& screenPoint) const { return _internal->ScreenToNdcPoint(screenPoint); }

        /** @copydoc Camera::viewToWorldPoint */
        Vector3 viewToWorldPoint(const Vector3& viewPoint) const { UpdateView(); return _internal->ViewToWorldPoint(viewPoint); }

        /** @copydoc Camera::viewToScreenPoint */
        Vector2I viewToScreenPoint(const Vector3& viewPoint) const { return _internal->ViewToScreenPoint(viewPoint); }

        /** @copydoc Camera::viewToNdcPoint */
        Vector2 viewToNdcPoint(const Vector3& viewPoint) const { return _internal->ViewToNdcPoint(viewPoint); }

        /** @copydoc Camera::ndcToWorldPoint */
        Vector3 ndcToWorldPoint(const Vector2& ndcPoint, float depth = 0.5f) const { UpdateView(); return _internal->NdcToWorldPoint(ndcPoint, depth); }

        /** @copydoc Camera::ndcToViewPoint */
        Vector3 ndcToViewPoint(const Vector2& ndcPoint, float depth = 0.5f) const { return _internal->NdcToViewPoint(ndcPoint, depth); }

        /** @copydoc Camera::ndcToScreenPoint */
        Vector2I ndcToScreenPoint(const Vector2& ndcPoint) const { return _internal->NdcToScreenPoint(ndcPoint); }

        /** @copydoc Camera::screenPointToRay */
        Ray screenPointToRay(const Vector2I& screenPoint) const { UpdateView(); return _internal->ScreenPointToRay(screenPoint); }

        /** @copydoc Camera::projectPoint */
        Vector3 projectPoint(const Vector3& point) const { return _internal->ProjectPoint(point); }

        /** @copydoc Camera::unprojectPoint */
        Vector3 unprojectPoint(const Vector3& point) const { return _internal->UnprojectPoint(point); }

        /** @copydoc Camera::SetMain */
        void SetMain(bool main);

        /** @copydoc Camera::isMain */
        bool isMain() const { return _internal->IsMain(); }

        /** Returns the internal camera that is used for majority of operations by this component. */
        SPtr<Camera> _getCamera() const { UpdateView(); return _internal; }

    protected:
        /** Checks if the world transform of the camera changed, and if needed updates the view matrix. */
        void UpdateView() const;

    protected:
        mutable SPtr<Camera> _internal;

    protected:
        friend class SceneObject;

        CCamera(const HSceneObject& parent);

        /** @copydoc Component::_instantiate */
        void _instantiate() override;

        /** @copydoc Component::OnInitialized */
        void OnCreated() override;

        /** @copydoc Component::OnInitialized */
        void OnInitialized() override;

        /** @copydoc Component::OnTransformChanged */
        void OnTransformChanged(TransformChangedFlags flags) override;

        /** @copydoc Component::OnDestroyed */
        void OnDestroyed() override;

    public:
        /** @copydoc Component::Update */
        void Update() override { }

    protected:
        CCamera();
    };
}
