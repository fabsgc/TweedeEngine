#include "Components/TeCRenderable.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    CRenderable::CRenderable()
        : Component(HSceneObject(), (UINT32)TID_CRenderable)
    {
        SetName("Renderable");
    }

    CRenderable::CRenderable(const HSceneObject& parent)
        : Component(parent, (UINT32)TID_CRenderable)
    {
        SetName("Renderable");
    }

    void CRenderable::Initialize()
    {
        Component::Initialize();
    }

    void CRenderable::SetMaterials(const Vector<HMaterial>& materials)
    {
        for (auto& material : materials)
        {
            SetMaterial(material);
        }
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

    void CRenderable::_instantiate()
    {
        _internal = Renderable::Create();
    }

    void CRenderable::OnCreated()
    { }

    void CRenderable::OnInitialized()
    {
        gSceneManager()._bindActor(_internal, GetSceneObject());
    }

    void CRenderable::OnTransformChanged(TransformChangedFlags flags)
    {
        _internal->_updateState(*SO());
    }

    void CRenderable::OnDestroyed()
    {
        gSceneManager()._unbindActor(_internal);
        _internal->Destroy();
    }
}
