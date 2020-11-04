#include "Components/TeCCamera.h"
#include "Scene/TeSceneManager.h"

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
        if (!_internal->IsDestroyed())
            _internal->Destroy();
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
        gSceneManager()._notifyMainCameraStateChanged(_internal);

        Component::OnInitialized();
    }

    void CCamera::OnTransformChanged(TransformChangedFlags flags)
    {
        _internal->_updateState(*SO());
    }

    void CCamera::OnDestroyed()
    {
        gSceneManager()._unbindActor(_internal);
        Component::OnDestroyed();
        _internal->Destroy();
    }

    void CCamera::Clone(const HComponent& c)
    {
        Clone(static_object_cast<CCamera>(c));
    }

    void CCamera::Clone(const HCamera& c)
    {
        Component::Clone(c.GetInternalPtr());
        SPtr<Camera> camera = c->_getCamera();

        _internal->_transform = camera->_transform;
        _internal->_mobility = camera->_mobility;
    }
}
