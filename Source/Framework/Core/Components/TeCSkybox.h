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
        virtual ~CSkybox();

        /** Return Component type */
        static UINT32 GetComponentType() { return TypeID_Core::TID_CSkybox; }

        /** @copydoc Component::Clone */
        bool Clone(const HComponent& c, const String& suffix = "") override;

        /** @copydoc Component::Clone */
        bool Clone(const HSkybox& c, const String& suffix = "");

        /** @copydoc Skybox::GetTexture */
        SPtr<Texture> GetTexture() const { return _internal->GetTexture(); }

        /** @copydoc Skybox::SetTexture */
        void SetTexture(const HTexture& texture) { _internal->SetTexture(texture); }

        /** @copydoc Skybox::SetTexture */
        void SetTexture(const SPtr<Texture>& texture) { _internal->SetTexture(texture); }

        /** @copydoc Skybox::GetDiffuseIrradiance */
        SPtr<Texture> GetDiffuseIrradiance() const { return _internal->GetDiffuseIrradiance(); }

        /** @copydoc Skybox::GetSpecularIrradiance */
        SPtr<Texture> GetSpecularIrradiance() const { return _internal->GetSpecularIrradiance(); }

        /** @copydoc Skybox::SetBrightness */
        void SetBrightness(float brightness) { _internal->SetBrightness(brightness); }

        /** @copydoc Skybox::GetBrightness */
        float GetBrightness() const { return _internal->GetBrightness(); }

        /** @copydoc Skybox::SetIBLIntensity */
        void SetIBLIntensity(float IBLIntensity) { _internal->SetIBLIntensity(IBLIntensity); }

        /** @copydoc Skybox::GetIBLIntensity */
        float GetIBLIntensity() const { return _internal->GetIBLIntensity(); }

        /**	Returns the skybox that this component wraps. */
        SPtr<Skybox> GetInternal() const { return _internal; }

        /* @copydoc Component::MarkDirty */
        virtual void MarkDirty() { _internal->_markCoreDirty(); }

    protected:
        mutable SPtr<Skybox> _internal;

    protected:
        friend class SceneObject;

        CSkybox();
        CSkybox(const HSceneObject& parent);

        /** @copydoc Component::Instantiate */
        void Instantiate() override;

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
    };
}
