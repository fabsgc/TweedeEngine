#include "Components/TeCLight.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CLight::CLight()
        : Component(HSceneObject(), (UINT32)TID_CLight)
    {
        SetFlag((UINT32)ComponentFlag::AlwaysRun, true);
        SetName("Light");
    }

    CLight::CLight(const HSceneObject& parent)
        : Component(parent, (UINT32)TID_CLight)
    {
        SetFlag((UINT32)ComponentFlag::AlwaysRun, true);
        SetName("Light");
    }

    CLight::~CLight()
    { }
}
