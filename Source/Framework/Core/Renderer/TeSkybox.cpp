#include "TeSkybox.h"
#include "Image/TeTexture.h"
#include "Image/TePixelUtil.h"
#include "Scene/TeSceneObject.h"
#include "Renderer/TeRenderer.h"
#include "Renderer/TeIBLUtility.h"
#include "RenderAPI/TeRenderAPI.h"

namespace te
{
    Skybox::Skybox()
        : Serializable(TID_Skybox)
    {
        // This shouldn't normally happen, as filtered textures are generated when a radiance texture is assigned, but
        // we check for it anyway (something could have gone wrong).
        if(_texture)
        {
            if (_filteredRadiance == nullptr || _irradiance == nullptr)
                FilterTexture();
        }
    }

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
        SetTexture((texture.IsLoaded()) ? texture.GetInternalPtr() : nullptr);
    }

    void Skybox::SetTexture(const SPtr<Texture>& texture)
    {
        if (!texture)
        {
            _texture = nullptr;
            _filteredRadiance = nullptr;
            _irradiance = nullptr;
        }
        else if(texture != _texture)
        {
            _texture = texture;
            FilterTexture();
        }

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

    void Skybox::FilterTexture()
    {
        {
            UINT32 size = IBLUtility::REFLECTION_CUBEMAP_SIZE;
            size = (size == _texture->GetProperties().GetWidth()) ? size / 2 : size;

            TEXTURE_DESC cubemapDesc;
            cubemapDesc.Type = TEX_TYPE_CUBE_MAP;
            cubemapDesc.Format = PF_RG11B10F;
            cubemapDesc.Width = size;
            cubemapDesc.Height = size;
            cubemapDesc.NumMips = PixelUtil::GetMaxMipmaps(cubemapDesc.Width, cubemapDesc.Height, 1);
            cubemapDesc.Usage = TU_STATIC | TU_RENDERTARGET;
            cubemapDesc.DebugName = "Filtered Radiance Texture";
            cubemapDesc.HwGamma = _texture->GetProperties().IsHardwareGammaEnabled();

            _filteredRadiance = Texture::CreatePtr(cubemapDesc);
        }

        {
            TEXTURE_DESC irradianceCubemapDesc;
            irradianceCubemapDesc.Type = TEX_TYPE_CUBE_MAP;
            irradianceCubemapDesc.Format = PF_RG11B10F;
            irradianceCubemapDesc.Width = IBLUtility::IRRADIANCE_CUBEMAP_SIZE;
            irradianceCubemapDesc.Height = IBLUtility::IRRADIANCE_CUBEMAP_SIZE;
            irradianceCubemapDesc.NumMips = 0;
            irradianceCubemapDesc.Usage = TU_STATIC | TU_RENDERTARGET;
            irradianceCubemapDesc.DebugName = "Irradiance Texture";
            irradianceCubemapDesc.HwGamma = _texture->GetProperties().IsHardwareGammaEnabled();

            _irradiance = Texture::CreatePtr(irradianceCubemapDesc);
        }

        RenderAPI& rapi = RenderAPI::Instance();

        rapi.PushMarker("[DRAW] FILTERED RADIANCE MAP", Color(0.67f, 0.49f, 0.26f));

        // Filter radiance
        gIBLUtility().ScaleCubemap(_texture, 0, _filteredRadiance, 0);
        gIBLUtility().FilterCubemapForSpecular(_filteredRadiance, nullptr);

        rapi.PopMarker();

        rapi.PushMarker("[DRAW] IRRADIANCE MAP", Color(0.23f, 0.48f, 0.55f));

        // Generate irradiance
        gIBLUtility().FilterCubemapForIrradiance(_texture, _irradiance);

        rapi.PopMarker();
    }

    const IBLUtility& gIBLUtility()
    {
        return IBLUtility::Instance();
    }
}
