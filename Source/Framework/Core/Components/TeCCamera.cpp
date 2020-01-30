#include "Components/TeCCamera.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CCamera::CCamera()
        : Component(HSceneObject(), (UINT32)TID_CCamera)
    {
        SetFlag((UINT32)ComponentFlag::AlwaysRun, true);
        SetName("Camera");
    }

    CCamera::CCamera(const HSceneObject& parent)
        : Component(parent, (UINT32)TID_CCamera)
    {
        SetFlag((UINT32)ComponentFlag::AlwaysRun, true);
        SetName("Camera");
    }

    CCamera::~CCamera()
    { }
}
