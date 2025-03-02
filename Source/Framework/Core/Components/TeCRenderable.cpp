#include "Components/TeCRenderable.h"

#include "Scene/TeSceneManager.h"
#include "Components/TeCAnimation.h"
#include "Renderer/TeRenderer.h"
#include "Serialization/TeUtility.h"
#include "Resources/TeResourceManager.h"

namespace te
{
    CRenderable::CRenderable()
        : Component(HSceneObject(), CoreType::TID_CRenderable)
    {
        SetName("Renderable");
        SetFlag(Component::AlwaysRun, true);
    }

    CRenderable::CRenderable(const HSceneObject& parent)
        : Component(parent, CoreType::TID_CRenderable)
    {
        SetName("Renderable");
        SetFlag(Component::AlwaysRun, true);
    }

    CRenderable::~CRenderable()
    {
        if (_internal && !_internal->IsDestroyed())
            _internal->Destroy();
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
        _internal->AttachTo(gRenderer());
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

    void CRenderable::ExportJson(nlohmann::json& document) const
    {
        Component::ExportJson(document);

        document["type"] = GetComponentType();
        
        if (_internal)
        {
            auto& renderableDoc = document["renderable"];
            renderableDoc["mesh"] = _internal->GetMesh().IsLoaded() ? serialization::GetResourceName(_internal->GetMesh().Get()) : "";
            renderableDoc["zPrepassMesh"] = _internal->GetZPrepassMesh().IsLoaded() ? serialization::GetResourceName(_internal->GetZPrepassMesh().Get()) : "";

            for (const auto& material : _internal->GetMaterials())
            {
                renderableDoc["materials"].push_back(material.IsLoaded() ? serialization::GetResourceName(material.Get()) : "");
            }

            renderableDoc["layer"] = _internal->GetLayer();
        }
    }

    bool CRenderable::ImportJson(const nlohmann::json& document, CRenderable& renderable)
    {
        Component::ImportJson(document, renderable);

        if (document.contains("renderable"))
        {
            const auto& renderableDoc = document["renderable"];
            if (renderableDoc.contains("mesh"))
            {
                HMesh mesh = static_resource_cast<Mesh>(gResourceManager().Get(serialization::GetResourceUUID(renderableDoc["mesh"].get<String>())));
                if (mesh.IsLoaded())
                    renderable._internal->SetMesh(mesh);
            }
            if (renderableDoc.contains("zPrepassMesh"))
            {
                HZPrepassMesh mesh = static_resource_cast<ZPrepassMesh>(gResourceManager().Get(serialization::GetResourceUUID(renderableDoc["zPrepassMesh"].get<String>())));
                if (mesh.IsLoaded())
                    renderable._internal->SetZPrepassMesh(mesh);
            }
            if (renderableDoc.contains("materials"))
            {
                uint32_t index = 0;
                for (const auto& material : renderableDoc["materials"])
                {
                    HMaterial mat = static_resource_cast<Material>(gResourceManager().Get(serialization::GetResourceUUID(material.get<String>())));
                    if (mat.IsLoaded())
                        renderable._internal->SetMaterial(index, mat);

                    index++;
                }
            }
            if (renderableDoc.contains("layer"))
            {
                renderable._internal->SetLayer(renderableDoc["layer"].get<UINT32>());
            }
        }

        return true;
    }
}
