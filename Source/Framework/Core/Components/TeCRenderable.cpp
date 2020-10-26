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

    CRenderable::~CRenderable()
    {
        if (!_internal->IsDestroyed())
            _internal->Destroy();
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

    void CRenderable::Clone(const HComponent& c)
    {
        Clone(static_object_cast<CRenderable>(c));
    }

    void CRenderable::Clone(const HRenderable& c)
    {
        SPtr<Renderable> renderable = c->_getInternal();

        _internal->_mesh = renderable->_mesh;
        _internal->_materials = renderable->_materials;
        _internal->_numMaterials = renderable->_numMaterials;
        _internal->_layer = renderable->_layer;
        _internal->_tfrmMatrix = renderable->_tfrmMatrix;
        _internal->_tfrmMatrixNoScale = renderable->_tfrmMatrixNoScale;
        _internal->_cullDistanceFactor = renderable->_cullDistanceFactor;
        _internal->_properties = renderable->_properties;
        _internal->_instancing = renderable->_instancing;
        _internal->_canBeMerged = renderable->_canBeMerged;

        _internal->_markCoreDirty(ActorDirtyFlag::GpuParams);

        Component::Clone(c.GetInternalPtr());
    }
}
