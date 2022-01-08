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
        if (_internal && !_internal->IsDestroyed())
            _internal->Destroy();
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
        _internal->UpdateState(*SO());
        return _internal->GetBounds();
    }

    bool CRenderable::CalculateBounds(Bounds& bounds)
    {
        bounds = GetBounds();
        return true;
    }

    void CRenderable::RegisterAnimation(const HAnimation& animation)
    {
        _animation = animation;

        if (_internal != nullptr)
        {
            _internal->SetAnimation(animation->GetInternal());

            // Need to update transform because animated renderables handle local transforms through bones, so it
            // shouldn't be included in the renderable's transform.
            _internal->UpdateState(*SO(), true);
        }
    }

    void CRenderable::UnregisterAnimation()
    {
        _animation = nullptr;

        if (_internal != nullptr)
        {
            _internal->SetAnimation(nullptr);

            // Need to update transform because animated renderables handle local transforms through bones, so it
            // shouldn't be included in the renderable's transform.
            _internal->UpdateState(*SO(), true);
        }
    }

    void CRenderable::Instantiate()
    {
        _internal = Renderable::Create();
    }

    void CRenderable::OnInitialized()
    {
        gSceneManager()._bindActor(_internal, SO());
        Component::OnInitialized();

        _animation = static_object_cast<CAnimation>(SO()->GetComponent<CAnimation>());
        if (_animation != nullptr)
        {
            RegisterAnimation(_animation);
            _animation->RegisterRenderable(static_object_cast<CRenderable>(_thisHandle));
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
        _internal->UpdateState(*SO());
    }

    void CRenderable::OnDestroyed()
    {
        if (_animation != nullptr)
            _animation->UnregisterRenderable();

        gSceneManager()._unbindActor(_internal);
        Component::OnDestroyed();
        _internal->Destroy();
    }

    bool CRenderable::Clone(const HComponent& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        return Clone(static_object_cast<CRenderable>(c), suffix);
    }

    bool CRenderable::Clone(const HRenderable& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        if (Component::Clone(c.GetInternalPtr(), suffix))
        {
            SPtr<Renderable> renderable = c->GetInternal();
            if (renderable)
            {
                _internal->_mesh = renderable->_mesh;
                _internal->_materials = renderable->_materials;
                _internal->_numMaterials = renderable->_numMaterials;
                _internal->_layer = renderable->_layer;
                _internal->_tfrmMatrix = renderable->_tfrmMatrix;
                _internal->_tfrmMatrixNoScale = renderable->_tfrmMatrixNoScale;
                _internal->_properties = renderable->_properties;
                _internal->_animation = renderable->_animation;
                _internal->_animationId = renderable->_animationId;

                _internal->_transform = renderable->_transform;
                _internal->_mobility = renderable->_mobility;

                _internal->_markCoreDirty(ActorDirtyFlag::GpuParams);
            }

            return true;
        }

        return false;
    }
}
