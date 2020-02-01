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

    void CRenderable::SetMesh(HMesh mesh)
    {
        _internal->SetMesh(mesh);
    }

    Bounds CRenderable::GetBounds() const
    {
        _internal->_updateState(*SO());
        return _internal->GetBounds();
    }

    bool CRenderable::CalculateBounds(Bounds& bounds)
    {
        bounds = GetBounds();
        return true;
    }

    void CRenderable::OnInitialized()
    {
        _internal = Renderable::Create();
        gSceneManager()._bindActor(_internal, GetSceneObject());
    }

    void CRenderable::OnDestroyed()
    {
        gSceneManager()._unbindActor(_internal);
        _internal->Destroy();
    }
}
