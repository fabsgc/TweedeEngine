#include "Components/TeCSkybox.h"
#include "Scene/TeSceneManager.h"
#include "Renderer/TeRenderer.h"
#include "Renderer/TeSkybox.h"
#include "Serialization/TeUtility.h"
#include "Resources/TeResourceManager.h"

namespace te
{
    CSkybox::CSkybox()
        : Component(HSceneObject(), CoreType::TID_CSkybox)
    {
        SetName("Skybox");
        SetFlag(Component::AlwaysRun, true);
        SetNotifyFlags(TCF_Parent | TCF_Transform);
    }

    CSkybox::CSkybox(const HSceneObject& parent)
        : Component(parent, CoreType::TID_CSkybox)
    {
        SetName("Skybox");
        SetFlag(Component::AlwaysRun, true);
        SetNotifyFlags(TCF_Parent | TCF_Transform);
    }

    CSkybox::~CSkybox()
    {
        if (_internal && !_internal->IsDestroyed())
            _internal->Destroy();
    }

    void CSkybox::Instantiate()
    {
        _internal = Skybox::Create();
        _internal->AttachTo(gRenderer());
    }

    void CSkybox::OnInitialized()
    {
        gSceneManager()._bindActor(_internal, GetSceneObject());
        Component::OnInitialized();
    }

    void CSkybox::OnEnabled()
    {
        _internal->SetActive(true);
        Component::OnEnabled();
    }

    void CSkybox::OnDisabled()
    {
        _internal->SetActive(false);
        Component::OnDisabled();
    }

    void CSkybox::OnDestroyed()
    {
        gSceneManager()._unbindActor(_internal);
        Component::OnDestroyed();

        if (_internal && !_internal->IsDestroyed())
            _internal->Destroy();
        _internal = nullptr;
    }

    bool CSkybox::Clone(const HComponent& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        return Clone(static_object_cast<CSkybox>(c), suffix);
    }

    bool CSkybox::Clone(const HSkybox& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        if (Component::Clone(c.GetInternalPtr(), suffix))
        {
            SPtr<Skybox> skybox = c->GetInternal();
            if (skybox)
            {
                _internal->_brightness = skybox->_brightness;
                _internal->_IBLIntensity = skybox->_IBLIntensity;
                _internal->_texture = skybox->_texture;
                _internal->_irradiance = skybox->_irradiance;
                _internal->_filteredRadiance = skybox->_filteredRadiance;

                _internal->_transform = skybox->_transform;
                _internal->_mobility = skybox->_mobility;

                _internal->MarkCoreDirty();
            }

            return true;
        }

        return false;
    }

    void CSkybox::ExportJson(nlohmann::json& document) const
    {
        Component::ExportJson(document);

        document["type"] = GetComponentType();

        if (_internal)
        {
            auto& skyboxDoc = document["skybox"];

            skyboxDoc["texture"] = serialization::GetResourceName(GetTexture().IsLoaded() ? GetTexture().Get() : nullptr);
            skyboxDoc["brightness"] = GetBrightness();
            skyboxDoc["IBLIntensity"] = GetIBLIntensity();
        }
    }

    bool CSkybox::ImportJson(const nlohmann::json& document, CSkybox& skybox)
    {
        Component::ImportJson(document, skybox);

        if (document.contains("skybox"))
        {
            const auto& skyboxDoc = document["skybox"];
            if (skyboxDoc.contains("texture"))
            {
                HTexture texture = static_resource_cast<Texture>(gResourceManager().Get(serialization::GetResourceUUID(skyboxDoc["texture"].get<String>())));
                skybox.SetTexture(texture);
            }
            if (skyboxDoc.contains("brightness"))
                skybox.SetBrightness(skyboxDoc["brightness"].get<float>());
            if (skyboxDoc.contains("IBLIntensity"))
                skybox.SetIBLIntensity(skyboxDoc["IBLIntensity"].get<float>());

            return true;
        }
        else
        {
            TE_DEBUG("Failed to import CSkybox from JSON, missing 'skybox' section.");
            return false;
        }
    }
}
