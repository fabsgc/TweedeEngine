#include "Components/TeCSkybox.h"
#include "Scene/TeSceneManager.h"
#include "Renderer/TeRenderer.h"
#include "Renderer/TeSkybox.h"

namespace te
{
    CSkybox::CSkybox()
        : Component(HSceneObject(), (UINT32)TID_CSkybox)
    {
        SetName("Skybox");
        SetFlag(Component::AlwaysRun, true);
        SetNotifyFlags(TCF_Parent | TCF_Transform);
    }

    CSkybox::CSkybox(const HSceneObject& parent)
        : Component(parent, (UINT32)TID_CSkybox)
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
        _internal->Destroy();
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
                _internal->_texture = skybox->_texture;
                _internal->_irradiance = skybox->_irradiance;

                _internal->_transform = skybox->_transform;
                _internal->_mobility = skybox->_mobility;

                _internal->MarkCoreDirty();
            }

            return true;
        }

        return false;
    }
}
