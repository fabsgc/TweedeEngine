#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeSkybox.h"
#include "Scene/TeComponent.h"
#include "Image/TeTexture.h"

namespace te
{
    /**
     * @copydoc	Skybox
     *
     * @note	Wraps Skybox as a Component.
     */
    class TE_CORE_EXPORT CSkybox : public Component
    {
    public:
        CSkybox(const HSceneObject& parent);
        virtual ~CSkybox();

        /** @copydoc Component::Initialize */
        void Initialize() override;

        /** @copydoc Skybox::getTexture */
        SPtr<Texture> GetTexture() const { return _internal->GetTexture(); }

        /** @copydoc Skybox::SetTexture */
        void SetTexture(const HTexture& texture) { _internal->SetTexture(texture); }

        /** @copydoc Skybox::SetTexture */
        void SetTexture(const SPtr<Texture>& texture) { _internal->SetTexture(texture); }

        /** @copydoc Skybox::SetBrightness */
        void setBrightness(float brightness) { _internal->SetBrightness(brightness); }

        /** @copydoc Skybox::getBrightness */
        float GetBrightness() const { return _internal->GetBrightness(); }

        /**	Returns the skybox that this component wraps. */
        SPtr<Skybox> _getSkybox() const { return _internal; }

        /** Return Component type */
        static UINT32 GetComponentType() { return TID_CSkybox; }

        /** @copydoc Component::Clone */
        void Clone(const HComponent& c) override;

        /** @copydoc Component::Clone */
        void Clone(const HSkybox& c);

    protected:
        mutable SPtr<Skybox> _internal;

    protected:
        friend class SceneObject;

        /** @copydoc Component::_instantiate */
        void _instantiate() override;

        /** @copydoc Component::OnInitialized */
        void OnCreated() override { }

        /** @copydoc Component::OnInitialized */
        void OnInitialized() override;

        /** @copydoc Component::OnEnabled */
        void OnEnabled() override;

        /** @copydoc Component::OnEnabled */
        void OnDisabled() override;

        /** @copydoc Component::OnTransformChanged */
        void OnTransformChanged(TransformChangedFlags flags) override { }

        /** @copydoc Component::OnDestroyed */
        void OnDestroyed() override;
    
    public:
        /** @copydoc Component::update */
        void Update() override { }

    protected:
        CSkybox();
    };
}
