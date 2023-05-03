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
        virtual ~CCamera();

        /** Return Component type */
        static UINT32 GetComponentType() { return TypeID_Core::TID_CCamera; }

        /** @copydoc Component::Clone */
        bool Clone(const HComponent& c, const String& suffix = "") override;

        /** @copydoc Component::Clone */
        bool Clone(const HCamera& c, const String& suffix = "");

        /** @copydoc Camera::SetFlags */
        void SetFlags(UINT32 flags) { _internal->SetFlags(flags); }

        /** @copydoc Camera::GetFlags */
        UINT32 GetFlags() const { return _internal->GetFlags(); }

        /** @copydoc Camera::GetViewport */
        SPtr<Viewport> GetViewport() const { return _internal->GetViewport(); }

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

        /** @copydoc Camera::SetFocalLength */
        virtual void SetFocalLength(float focalLength) { _internal->SetFocalLength(focalLength); }

        /** @copydoc Camera::GetFocalLength */
        virtual float GetFocalLength() const { return _internal->GetFocalLength(); }

        /** @copydoc Camera::SetAperture */
        virtual void SetAperture(float aperture) { _internal->SetAperture(aperture); }

        /** @copydoc Camera::GetAperture */
        virtual float GetAperture() const { return _internal->GetAperture(); }

        /** @copydoc Camera::SetShutterSpeed */
        virtual void SetShutterSpeed(float shutterSpeed) { _internal->SetShutterSpeed(shutterSpeed); }

        /** @copydoc Camera::GetShutterSpeed */
        virtual float GetShutterSpeed() const { return _internal->GetShutterSpeed(); }

        /** @copydoc Camera::SetSensitivity */
        virtual void SetSensitivity(UINT32 sensitivity) { _internal->SetSensitivity(sensitivity); }

        /** @copydoc Camera::GetSensitivity */
        virtual UINT32 GetSensitivity() const { return _internal->GetSensitivity(); }

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
        void SetLayers(UINT32 layers) { _internal->SetLayers(layers); }

        /** @copydoc Camera::GetLayers */
        UINT32 GetLayers() const { return _internal->GetLayers(); }

        /** @copydoc Camera::SetMSAACount */
        void SetMSAACount(UINT32 count) { _internal->SetMSAACount(count); }

        /** @copydoc Camera::GetMSAACount */
        UINT32 GetMSAACount() const { return _internal->GetMSAACount(); }

        /** @copydoc Camera::SetRenderSettings */
        void SetRenderSettings(const SPtr<RenderSettings>& settings) { _internal->SetRenderSettings(settings); }

        /** @copydoc Camera::SetRenderSettings */
        void SetRenderSettings(const RenderSettings& settings) { _internal->SetRenderSettings(settings); }

        /** @copydoc Camera::GetRenderSettings */
        const SPtr<RenderSettings>& GetRenderSettings() const { return _internal->GetRenderSettings(); }

        /** @copydoc Camera::NotifyNeedsRedraw */
        void NotifyNeedsRedraw() { _internal->NotifyNeedsRedraw(); }

        /** @copydoc Camera::NotifyNeedsRedraw */
        void NotifyUpdateEverything() { _internal->NotifyUpdateEverything(); }

        /** @copydoc Camera::WorldToScreenPoint */
        Vector2I WorldToScreenPoint(const Vector3& worldPoint) const { UpdateView(); return _internal->WorldToScreenPoint(worldPoint); }

        /** @copydoc Camera::WorldToNdcPoint */
        Vector2 WorldToNdcPoint(const Vector3& worldPoint) const { UpdateView(); return _internal->WorldToNdcPoint(worldPoint); }

        /** @copydoc Camera::WorldToViewPoint */
        Vector3 WorldToViewPoint(const Vector3& worldPoint) const { UpdateView(); return _internal->WorldToViewPoint(worldPoint); }

        /** @copydoc Camera::ScreenToWorldPoint */
        Vector3 ScreenToWorldPoint(const Vector2I& screenPoint, float depth = 0.5f) const { UpdateView(); return _internal->ScreenToWorldPoint(screenPoint, depth); }

        /** @copydoc Camera::ScreenToViewPoint */
        Vector3 ScreenToViewPoint(const Vector2I& screenPoint, float depth = 0.5f) const { return _internal->ScreenToViewPoint(screenPoint, depth); }

        /** @copydoc Camera::ScreenToNdcPoint */
        Vector2 ScreenToNdcPoint(const Vector2I& screenPoint) const { return _internal->ScreenToNdcPoint(screenPoint); }

        /** @copydoc Camera::ViewToWorldPoint */
        Vector3 ViewToWorldPoint(const Vector3& viewPoint) const { UpdateView(); return _internal->ViewToWorldPoint(viewPoint); }

        /** @copydoc Camera::ViewToScreenPoint */
        Vector2I ViewToScreenPoint(const Vector3& viewPoint) const { return _internal->ViewToScreenPoint(viewPoint); }

        /** @copydoc Camera::ViewToNdcPoint */
        Vector2 ViewToNdcPoint(const Vector3& viewPoint) const { return _internal->ViewToNdcPoint(viewPoint); }

        /** @copydoc Camera::NdcToWorldPoint */
        Vector3 NdcToWorldPoint(const Vector2& ndcPoint, float depth = 0.5f) const { UpdateView(); return _internal->NdcToWorldPoint(ndcPoint, depth); }

        /** @copydoc Camera::NdcToViewPoint */
        Vector3 NdcToViewPoint(const Vector2& ndcPoint, float depth = 0.5f) const { return _internal->NdcToViewPoint(ndcPoint, depth); }

        /** @copydoc Camera::NdcToScreenPoint */
        Vector2I NdcToScreenPoint(const Vector2& ndcPoint) const { return _internal->NdcToScreenPoint(ndcPoint); }

        /** @copydoc Camera::screenPointToRay */
        Ray ScreenPointToRay(const Vector2I& screenPoint) const { UpdateView(); return _internal->ScreenPointToRay(screenPoint); }

        /** @copydoc Camera::ProjectPoint */
        Vector3 ProjectPoint(const Vector3& point) const { return _internal->ProjectPoint(point); }

        /** @copydoc Camera::UnprojectPoint */
        Vector3 UnprojectPoint(const Vector3& point) const { return _internal->UnprojectPoint(point); }

        /** @copydoc Camera::GetTransform */
        const Transform& GetTransform() { return _internal->GetTransform(); }

        /** @copydoc Camera::SetMain */
        void SetMain(bool main);

        /** @copydoc Camera::IsMain */
        bool IsMain() const { return _internal->IsMain(); }

        /** @copydoc SceneActor::SetActive */
        virtual void SetActive(bool active) { _internal->SetActive(active); }

        /** @copydoc SceneActor::GetActive */
        bool GetActive() const { return _internal->GetActive(); }

        /** Returns the internal camera that is used for majority of operations by this component. */
        SPtr<Camera> GetInternal() const { UpdateView(); return _internal; }

        /* @copydoc Component::MarkDirty */
        virtual void MarkDirty() { _internal->_markCoreDirty(); }

    protected:
        /** Checks if the world transform of the camera changed, and if needed updates the view matrix. */
        void UpdateView() const;

    protected:
        mutable SPtr<Camera> _internal;

    protected:
        friend class SceneObject;

        CCamera();
        CCamera(const HSceneObject& parent);

        /** @copydoc Component::Instantiate */
        void Instantiate() override;

        /** @copydoc Component::OnInitialized */
        void OnInitialized() override;

        /** @copydoc Component::OnEnabled */
        void OnEnabled() override;

        /** @copydoc Component::OnDisabled */
        void OnDisabled() override;

        /** @copydoc Component::OnTransformChanged */
        void OnTransformChanged(TransformChangedFlags flags) override;

        /** @copydoc Component::OnDestroyed */
        void OnDestroyed() override;
    };
}
