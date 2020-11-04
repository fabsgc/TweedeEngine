#pragma once

#include "TeCorePrerequisites.h"
#include "Image/TeTexture.h"
#include "Scene/TeSceneActor.h"
#include "CoreUtility/TeCoreObject.h"

namespace te
{
    /**	Signals which portion of a Skybox is dirty. */
    enum class SkyboxDirtyFlag
    {
        // First few bits reserved by ActorDiryFlag
        Texture = 1 << 5
    };

    class TE_CORE_EXPORT Skybox : public CoreObject, public SceneActor
    {
    public:
        ~Skybox();

        /**
         * Brightness multiplier that will be applied to skybox values before they're being used. Allows you to make the
         * skybox more or less bright. Equal to one by default.
         */
        void SetBrightness(float brightness) { _brightness = brightness; _markCoreDirty(); }

        /** @copydoc setBrightness */
        float GetBrightness() const { return _brightness; }

        /**
         * Determines an environment map to use for sampling skybox radiance. Must be a cube-map texture, and should ideally
         * contain HDR data.
         */
        SPtr<Texture> GetTexture() const { return _texture; }

        /** @copydoc Skybox::GetTexture */
        void SetTexture(const HTexture& texture);

        /** @copydoc Skybox::GetTexture */
        void SetTexture(const SPtr<Texture>& texture);

        /** Creates a new skybox. */
        static SPtr<Skybox> Create();

        /**	Creates a new skybox instance without initializing it. */
        static SPtr<Skybox> CreateEmpty();

    protected:
        friend class CSkybox;

        Skybox();
        Skybox(const SPtr<Texture>& radiance);

        /** @copydoc CoreObject::_markCoreDirty */
        void _markCoreDirty(ActorDirtyFlag flag = ActorDirtyFlag::Everything) override;

        /** @copydoc CoreObject::FrameSync */
        void FrameSync() override;

        /** @copydoc CoreObject::initialize */
        void Initialize() override;

    protected:
        float _brightness = 1.0f; /**< Multiplier to apply to evaluated skybox values before using them. */
        SPtr<Texture> _texture;
    };
}
