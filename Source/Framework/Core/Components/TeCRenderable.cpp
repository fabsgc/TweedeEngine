#include "Components/TeCRenderable.h"
#include "Scene/TeSceneManager.h"
#include "Components/TeCAnimation.h"

namespace te
{
    CRenderable::CRenderable()
        : Component(HSceneObject(), (UINT32)TID_CRenderable)
    {
        SetName("Renderable");
        SetFlag(Component::AlwaysRun, true);
    }

    CRenderable::CRenderable(const HSceneObject& parent)
        : Component(parent, (UINT32)TID_CRenderable)
    {
        SetName("Renderable");
        SetFlag(Component::AlwaysRun, true);
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

    void CRenderable::_registerAnimation(const HAnimation& animation)
    {
        _animation = animation;

        if (_internal != nullptr)
        {
            _internal->SetAnimation(animation->_getInternal());

            // Need to update transform because animated renderables handle local transforms through bones, so it
            // shouldn't be included in the renderable's transform.
            _internal->_updateState(*SO(), true);
        }
    }

    void CRenderable::_unregisterAnimation()
    {
        _animation = nullptr;

        if (_internal != nullptr)
        {
            _internal->SetAnimation(nullptr);

            // Need to update transform because animated renderables handle local transforms through bones, so it
            // shouldn't be included in the renderable's transform.
            _internal->_updateState(*SO(), true);
        }
    }

    void CRenderable::_instantiate()
    {
        _internal = Renderable::Create();
    }

    void CRenderable::OnCreated()
    {
        Component::OnCreated();
    }

    void CRenderable::OnInitialized()
    {
        gSceneManager()._bindActor(_internal, GetSceneObject());
        Component::OnInitialized();

        _animation = static_object_cast<CAnimation>(SO()->GetComponent<CAnimation>());
        if (_animation != nullptr)
        {
            _registerAnimation(_animation);
            _animation->_registerRenderable(static_object_cast<CRenderable>(_thisHandle));
        }
    }

    void CRenderable::OnEnabled()
    {
        _internal->SetActive(true);
        Component::OnEnabled();
    }

    void CRenderable::OnDisabled()
    {
        _internal->SetActive(false);
        Component::OnDisabled();
    }

    void CRenderable::OnTransformChanged(TransformChangedFlags flags)
    {
        _internal->_updateState(*SO());
    }

    void CRenderable::OnDestroyed()
    {
        if (_animation != nullptr)
            _animation->_unregisterRenderable();

        gSceneManager()._unbindActor(_internal);
        Component::OnDestroyed();
        _internal->Destroy();
    }

    void CRenderable::Clone(const HComponent& c)
    {
        Clone(static_object_cast<CRenderable>(c));
    }

    void CRenderable::Clone(const HRenderable& c)
    {
        Component::Clone(c.GetInternalPtr());
        SPtr<Renderable> renderable = c->_getInternal();

        _internal->_mesh = renderable->_mesh;
        _internal->_materials = renderable->_materials;
        _internal->_numMaterials = renderable->_numMaterials;
        _internal->_layer = renderable->_layer;
        _internal->_tfrmMatrix = renderable->_tfrmMatrix;
        _internal->_tfrmMatrixNoScale = renderable->_tfrmMatrixNoScale;
        _internal->_properties = renderable->_properties;

        _internal->_transform = renderable->_transform;
        _internal->_mobility = renderable->_mobility;

        _internal->_markCoreDirty(ActorDirtyFlag::GpuParams);
    }
}
