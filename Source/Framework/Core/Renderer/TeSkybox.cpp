#include "TeSkybox.h"
#include "Scene/TeSceneObject.h"
#include "Image/TeTexture.h"
#include "Renderer/TeRenderer.h"

namespace te
{
    Skybox::Skybox()
        : _texture(nullptr)
    { }

    Skybox::Skybox(const SPtr<Texture>& radiance)
        : _texture(radiance)
    { }

    Skybox::~Skybox()
    {
        if(_active)
            gRenderer()->NotifySkyboxRemoved(this);
    }

    void Skybox::Initialize()
    {
        gRenderer()->NotifySkyboxAdded(this);
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
                gRenderer()->NotifySkyboxAdded(this);
            else
                gRenderer()->NotifySkyboxRemoved(this);
        }
        else if ((dirtyFlag & ((UINT32)SkyboxDirtyFlag::Texture)) != 0)
        {
            gRenderer()->NotifySkyboxRemoved(this);
            gRenderer()->NotifySkyboxAdded(this);
        }

        _oldActive = _active;
    }
}
