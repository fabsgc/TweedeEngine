#include "Components/TeCRenderable.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CRenderable::CRenderable()
        : Component(HSceneObject(), (UINT32)TID_CRenderable)
    {
        SetFlag((UINT32)ComponentFlag::AlwaysRun, true);
        SetName("Renderable");
    }

    CRenderable::CRenderable(const HSceneObject& parent)
        : Component(parent, (UINT32)TID_CRenderable)
    {
        SetFlag((UINT32)ComponentFlag::AlwaysRun, true);
        SetName("Renderable");
    }

    CRenderable::~CRenderable()
    { }
}
