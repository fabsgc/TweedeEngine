#include "Components/TeCCamera.h"
#include "Scene/TeSceneManager.h"
#include "Renderer/TeRenderer.h"

namespace te
{
    CCamera::CCamera()
        : Component(HSceneObject(), CoreType::TID_CCamera)
    {
        SetName("Camera");
        SetFlag(Component::AlwaysRun, true);
    }

    CCamera::CCamera(const HSceneObject& parent)
        : Component(parent, CoreType::TID_CCamera)
    {
        SetName("Camera");
        SetFlag(Component::AlwaysRun, true);
    }

    CCamera::~CCamera()
    {
        if (_internal && !_internal->IsDestroyed())
            _internal->Destroy();
    }

    ConvexVolume CCamera::GetWorldFrustum() const
    {
        const Vector<Plane>& frustumPlanes = GetFrustum().GetPlanes();
        Matrix4 worldMatrix = SO()->GetWorldMatrix();

        Vector<Plane> worldPlanes(frustumPlanes.size());
        UINT32 i = 0;
        for (auto& plane : frustumPlanes)
        {
            worldPlanes[i] = worldMatrix.MultiplyAffine(plane);
            i++;
        }

        return ConvexVolume(worldPlanes);
    }

    void CCamera::UpdateView() const
    {
        _internal->UpdateState(*SO());
    }

    void CCamera::SetMain(bool main)
    {
        _internal->SetMain(main);
    }

    void CCamera::Instantiate()
    {
        _internal = Camera::Create();
        _internal->AttachTo(gRenderer());
    }

    void CCamera::OnInitialized()
    {
        gSceneManager()._bindActor(_internal, SO());
        gSceneManager()._notifyMainCameraStateChanged(_internal);

        Component::OnInitialized();
    }

    void CCamera::OnEnabled()
    {
        _internal->SetActive(true);
        Component::OnEnabled();
    }

    void CCamera::OnDisabled()
    {
        _internal->SetActive(false);
        Component::OnDisabled();
    }

    void CCamera::OnTransformChanged(TransformChangedFlags flags)
    {
        _internal->UpdateState(*SO());
    }

    void CCamera::OnDestroyed()
    {
        gSceneManager()._unbindActor(_internal);
        Component::OnDestroyed();
        _internal->Destroy();
    }

    bool CCamera::Clone(const HComponent& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        return Clone(static_object_cast<CCamera>(c), suffix);
    }

    bool CCamera::Clone(const HCamera& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        if (Component::Clone(c.GetInternalPtr(), suffix))
        {
            SPtr<Camera> camera = c->GetInternal();
            if(camera)
            {
                _internal->_transform = camera->_transform;
                _internal->_mobility = camera->_mobility;
            }

            return true;
        }

        return false;
    }

    void CCamera::ExportJson(nlohmann::json& document) const
    {
        Component::ExportJson(document);

        document["type"] = GetComponentType();
        document["nearClipDistance"] = GetNearClipDistance();
        document["farClipDistance"] = GetFarClipDistance();
        document["aspectRatio"] = GetAspectRatio();
        document["focalLength"] = GetFocalLength();
        document["focalLength"] = GetFocalLength();
        document["aperture"] = GetAperture();
        document["shutterSpeed"] = GetShutterSpeed();
        document["sensitivity"] = GetSensitivity();
        document["sensitivity"] = GetSensitivity();
        document["projectionType"] = GetProjectionType();
        document["orthoWindowHeight"] = GetOrthoWindowHeight();
        document["orthoWindowWidth"] = GetOrthoWindowWidth();
        document["priority"] = GetPriority();
        document["layers"] = GetLayers();
        document["MSAACount"] = GetMSAACount();
        document["main"] = IsMain();
        
        GetRenderSettings()->ExportJson(document["renderSettings"]);
    }

    bool CCamera::ImportJson(const nlohmann::json& document, CCamera& camera)
    {
        Component::ImportJson(document, camera);

        // Camera properties
        if (document.contains("nearClipDistance")) camera.SetNearClipDistance(document["nearClipDistance"].get<float>());
        if (document.contains("farClipDistance")) camera.SetFarClipDistance(document["farClipDistance"].get<float>());
        if (document.contains("aspectRatio")) camera.SetAspectRatio(document["aspectRatio"].get<float>());
        if (document.contains("focalLength")) camera.SetFocalLength(document["focalLength"].get<float>());
        if (document.contains("aperture")) camera.SetAperture(document["aperture"].get<float>());
        if (document.contains("shutterSpeed")) camera.SetShutterSpeed(document["shutterSpeed"].get<float>());
        if (document.contains("sensitivity")) camera.SetSensitivity(document["sensitivity"].get<uint32_t>());
        if (document.contains("projectionType")) camera.SetProjectionType(static_cast<ProjectionType>(document["projectionType"].get<int>()));
        if (document.contains("orthoWindowHeight")) camera.SetOrthoWindowHeight(document["orthoWindowHeight"].get<float>());
        if (document.contains("orthoWindowWidth")) camera.SetOrthoWindowWidth(document["orthoWindowWidth"].get<float>());
        if (document.contains("priority")) camera.SetPriority(document["priority"].get<int32_t>());
        if (document.contains("layers")) camera.SetLayers(document["layers"].get<uint32_t>());
        if (document.contains("MSAACount")) camera.SetMSAACount(document["MSAACount"].get<uint32_t>());
        if (document.contains("main")) camera.SetMain(document["main"].get<bool>());

        // RenderSettings
        if (document.contains("renderSettings"))
        {
            auto settings = camera.GetRenderSettings();
            RenderSettings::ImportJson(document["renderSettings"], *settings);
            camera.SetRenderSettings(settings);
        }

        return true;
    }
}
