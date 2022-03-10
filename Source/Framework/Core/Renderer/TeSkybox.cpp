#include "TeSkybox.h"
#include "Image/TeTexture.h"
#include "Scene/TeSceneObject.h"
#include "Renderer/TeRenderer.h"

namespace te
{
    Skybox::Skybox()
        : Serializable(TID_Skybox)
    { }

    Skybox::Skybox(const SPtr<Texture>& radiance)
        : Serializable(TID_Skybox)
        , _texture(radiance)
    { }

    Skybox::~Skybox()
    {
        if(_active)
        {
            if (_renderer) _renderer->NotifySkyboxRemoved(this);
        }
    }

    void Skybox::Initialize()
    {
        if (_renderer) _renderer->NotifySkyboxAdded(this);
        CoreObject::Initialize();
    }

    void Skybox::SetTexture(const HTexture& texture)
    {
        _texture = texture.GetInternalPtr();
        _markCoreDirty((ActorDirtyFlag)SkyboxDirtyFlag::Texture);
    }

    void Skybox::SetTexture(const SPtr<Texture>& texture)
    {
        _texture = texture;
        _markCoreDirty((ActorDirtyFlag)SkyboxDirtyFlag::Texture);
    }

    void Skybox::SetDiffuseIrradiance(const HTexture& irradiance)
    {
        _irradiance = irradiance.GetInternalPtr();
        _markCoreDirty((ActorDirtyFlag)SkyboxDirtyFlag::Texture);
    }

    void Skybox::SetDiffuseIrradiance(const SPtr<Texture>& irradiance)
    {
        _irradiance = irradiance;
        _markCoreDirty((ActorDirtyFlag)SkyboxDirtyFlag::Texture);
    }

    void Skybox::AttachTo(SPtr<Renderer> renderer)
    {
        if (_renderer)
            _renderer->NotifySkyboxRemoved(this);

        _renderer = renderer;

        if (_renderer)
            _renderer->NotifySkyboxAdded(this);

        _markCoreDirty();
    }

    SPtr<Skybox> Skybox::CreateEmpty()
    {
        Skybox* skybox = new (te_allocate<Skybox>()) Skybox();
        SPtr<Skybox> skyboxPtr = te_core_ptr<Skybox>(skybox);
        skyboxPtr->SetThisPtr(skyboxPtr);

        return skyboxPtr;
    }

    SPtr<Skybox> Skybox::Create()
    {
        SPtr<Skybox> skyboxPtr = CreateEmpty();
        skyboxPtr->Initialize();

        return skyboxPtr;
    }

    void Skybox::_markCoreDirty(ActorDirtyFlag flags)
    {
        MarkCoreDirty((UINT32)flags);
    }

    void Skybox::FrameSync()
    {
        UINT32 dirtyFlag = GetCoreDirtyFlags();

        if (_oldActive != _active)
        {
            if (_active)
            {
                if (_renderer) _renderer->NotifySkyboxAdded(this);
            }
            else
            {
                if (_renderer) _renderer->NotifySkyboxRemoved(this);
            }
        }
        else if ((dirtyFlag & ((UINT32)SkyboxDirtyFlag::Texture)) != 0)
        {
            if (_renderer) _renderer->NotifySkyboxRemoved(this);
            if (_renderer) _renderer->NotifySkyboxAdded(this);
        }

        _oldActive = _active;
    }
}
