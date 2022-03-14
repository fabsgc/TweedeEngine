#pragma once

#include "TeCorePrerequisites.h"
#include "Scene/TeSceneActor.h"
#include "CoreUtility/TeCoreObject.h"
#include "Serialization/TeSerializable.h"

namespace te
{
    /**	Signals which portion of a Skybox is dirty. */
    enum class SkyboxDirtyFlag
    {
        // First few bits reserved by ActorDiryFlag
        Texture = 1 << 5
    };

    class TE_CORE_EXPORT Skybox : public CoreObject, public SceneActor, public Serializable
    {
    public:
        virtual ~Skybox();

        /**
         * Brightness multiplier that will be applied to skybox values before they're being used. Allows you to make the
         * skybox more or less bright. Equal to one by default.
         */
        void SetBrightness(float brightness) { _brightness = brightness; _markCoreDirty(); }

        /** @copydoc setBrightness */
        float GetBrightness() const { return _brightness; }

        /** @copydoc Skybox::GetTexture */
        void SetTexture(const HTexture& texture);

        /** @copydoc Skybox::GetTexture */
        void SetTexture(const SPtr<Texture>& texture);

        /**
         * Determines an environment map to use for sampling skybox radiance. Must be a cube-map texture, and should ideally
         * contain HDR data.
         */
        SPtr<Texture> GetTexture() const { return _texture; }

        /** @copydoc Skybox::GetDiffuseIrradiance */
        void SetDiffuseIrradiance(const HTexture& irradiance);

        /** @copydoc Skybox::GetDiffuseIrradiance */
        void SetDiffuseIrradiance(const SPtr<Texture>& irradiance);

        /**
         * Determines an environment map to use for sampling skybox irradiance. Must be a cube-map texture, and should ideally
         * contain HDR data.
         */
        SPtr<Texture> GetDiffuseIrradiance() const { return _irradiance; }

        /**
         * You can change at runtime which renderer will handle this skybox
         * Current renderer will be notified that skybox must be removed
         * And next renderer will be notified that skybox must be added
         */
        void AttachTo(SPtr<Renderer> renderer = nullptr);

        /** Creates a new skybox. */
        static SPtr<Skybox> Create();

        /**	Creates a new skybox instance without initializing it. */
        static SPtr<Skybox> CreateEmpty();

    protected:
        /** @copydoc SceneActor::_markCoreDirty */
        void _markCoreDirty(ActorDirtyFlag flag = ActorDirtyFlag::Everything) override;

        /** @copydoc CoreObject::FrameSync */
        void FrameSync() override;

        /**
         * Filters the skybox radiance texture, generating filtered radiance (for reflections) and irradiance. Should be
         * called any time the skybox texture changes.
         */
        void FilterTexture();

    protected:
        friend class CSkybox;

        Skybox();

        /** @copydoc CoreObject::Initialize */
        void Initialize() override;

    protected:
        float _brightness = 1.0f; /**< Multiplier to apply to evaluated skybox values before using them. */

        /** Texture used to display in 3D scene */
        SPtr<Texture> _texture;

        /** Texture used to compute IBL specular irradiance */
        SPtr<Texture> _filteredRadiance;

        /** Texture used to compute IBL diffuse irradiance */
        SPtr<Texture> _irradiance;

        /** Default renderer if this attributes is not filled in constructor. */
        SPtr<Renderer> _renderer; 
    };
}
