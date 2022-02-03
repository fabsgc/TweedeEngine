#include "Components/TeCCamera.h"
#include "Scene/TeSceneManager.h"
#include "Renderer/TeRenderer.h"

namespace te
{
    CCamera::CCamera()
        : Component(HSceneObject(), (UINT32)TID_CCamera)
    {
        SetName("Camera");
        SetFlag(Component::AlwaysRun, true);
    }

    CCamera::CCamera(const HSceneObject& parent)
        : Component(parent, (UINT32)TID_CCamera)
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
}
