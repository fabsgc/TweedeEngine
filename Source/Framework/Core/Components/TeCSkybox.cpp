#include "Components/TeCSkybox.h"
#include "Scene/TeSceneManager.h"
#include "Renderer/TeSkybox.h"

namespace te
{
    CSkybox::CSkybox()
        : Component(HSceneObject(), (UINT32)TID_CSkybox)
    {
        SetName("Skybox");
    }

    CSkybox::CSkybox(const HSceneObject& parent)
        : Component(parent, (UINT32)TID_CSkybox)
    {
        SetName("Skybox");
    }

    CSkybox::~CSkybox()
    {
        if (!_internal->IsDestroyed())
            _internal->Destroy();
    }

    void CSkybox::_instantiate()
    {
        _internal = Skybox::Create();
    }

    void CSkybox::Initialize()
    {
        Component::Initialize();
    }

    void CSkybox::OnInitialized()
    {
        gSceneManager()._bindActor(_internal, GetSceneObject());
    }

    void CSkybox::OnDestroyed()
    {
        gSceneManager()._unbindActor(_internal);
    }

    void CSkybox::Clone(const HComponent& c)
    {
        Clone(static_object_cast<CSkybox>(c));
    }

    void CSkybox::Clone(const HSkybox& c)
    {
        Component::Clone(c.GetInternalPtr());
        SPtr<Skybox> skybox = c->_getSkybox();

        _internal->_brightness = skybox->_brightness;
        _internal->_texture = skybox->_texture;

        _internal->_transform = skybox->_transform;
        _internal->_mobility = skybox->_mobility;
    }
}
