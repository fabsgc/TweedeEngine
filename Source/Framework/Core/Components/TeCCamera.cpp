#include "Components/TeCCamera.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CCamera::CCamera()
        : Component(HSceneObject(), (UINT32)TID_CCamera)
    {
        SetName("Camera");
    }

    CCamera::CCamera(const HSceneObject& parent)
        : Component(parent, (UINT32)TID_CCamera)
    {
        SetName("Camera");
    }

    void CCamera::Initialize()
    {
        Component::Initialize();
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
        _internal->_updateState(*SO());
    }

    void CCamera::SetMain(bool main)
    {
        _internal->SetMain(main);
    }

    void CCamera::_instantiate()
    {
        _internal = Camera::Create();
    }

    void CCamera::OnInitialized()
    {
        gSceneManager()._bindActor(_internal, SO());

        // Make sure primary RT gets applied if camera gets deserialized with main camera state
        gSceneManager()._notifyMainCameraStateChanged(_internal);
    }

    void CCamera::OnDestroyed()
    {
        gSceneManager()._unbindActor(_internal);
        _internal->Destroy();
    }
}
