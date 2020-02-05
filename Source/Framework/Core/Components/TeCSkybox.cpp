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
}
