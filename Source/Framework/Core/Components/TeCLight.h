#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeLight.h"
#include "Scene/TeComponent.h"

namespace te
{
    /**
     * @copydoc	Light
     *
     * @note	Wraps Light as a Component.
     */
    class TE_CORE_EXPORT CLight : public Component
    {
    public:
        CLight(const HSceneObject& parent, LightType type = LightType::Directional, Color color = Color::White,
            float intensity = Light::DefaultIntensity, float range = Light::DefaultAttRadius, bool castShadows = Light::DefaultCastShadow, 
            Degree spotAngle = Degree(Light::DefaultSpotAngle));

        virtual ~CLight();

        /** @copydoc Component::Initialize */
        void Initialize() override;

        /** @copydoc Light::SetType */
        void SetType(LightType type) { _internal->SetType(type); }

        /** @copydoc Light::GetType */
        LightType GetType() const { return _internal->GetType(); }

        /** @copydoc Light::SetColor */
        void SetColor(const Color& color) { _internal->SetColor(color); }

        /** @copydoc Light::GetColor */
        Color GetColor() const { return _internal->GetColor(); }

        /** @copydoc Light::SetIntensity */
        void SetIntensity(float intensity) { _internal->SetIntensity(intensity); }

        /** @copydoc Light::GetIntensity */
        float GetIntensity() const { return _internal->GetIntensity(); }

        /** @copydoc Light::SetAttenuationRadius */
        void SetAttenuationRadius(float radius) { _internal->SetAttenuationRadius(radius); }

        /** @copydoc Light::GetAttenuationRadius */
        float GetAttenuationRadius() const { return _internal->GetAttenuationRadius(); }

        /** @copydoc Light::SetLinearAttenuation */
        void SetLinearAttenuation(float attenuation) { _internal->SetLinearAttenuation(attenuation); }

        /** @copydoc Light::GetLinearAttenuation */
        float GetLinearAttenuation() const { return _internal->GetLinearAttenuation(); }

        /** @copydoc Light::SetQuadraticAttenuation */
        void SetQuadraticAttenuation(float attenuation) { _internal->SetQuadraticAttenuation(attenuation); }

        /** @copydoc Light::GetQuadraticAttenuation */
        float GetQuadraticAttenuation() const { return _internal->GetQuadraticAttenuation(); }

        /** @copydoc Light::SetSpotAngle */
        void SetSpotAngle(const Degree& spotAngle) { _internal->SetSpotAngle(spotAngle); }

        /** @copydoc Light::GetSpotAngle */
        Degree GetSpotAngle() const { return _internal->GetSpotAngle(); }

        /** @copydoc Light::SetCastShadows */
        void SetCastShadows(bool castShadows) { _internal->SetCastShadows(castShadows); }

        /** @copydoc Light::GetCastShadows */
        bool GetCastShadows() const { return _internal->GetCastShadows(); }

        /** @copydoc Light::SetShadowBias */
        void SetShadowBias(float bias) { _internal->SetShadowBias(bias); }

        /** @copydoc Light::SetShadowBias */
        float GetShadowBias() const { return _internal->GetShadowBias(); }

        /** @copydoc Light::GetBounds */
        Sphere GetBounds() const;

        /** @copydoc Light::GetTransform */
        const Transform& GetTransform() { return _internal->GetTransform(); }

        /** @copydoc SceneActor::SetActive */
        virtual void SetActive(bool active) { _internal->SetActive(active); }

        /** @copydoc SceneActor::GetActive */
        bool GetActive() const { return _internal->GetActive(); }

        /** Returns the light that this component wraps. */
        SPtr<Light> _getLight() const { return _internal; }

        static UINT32 GetComponentType() { return TID_CLight; }

        /** @copydoc Component::Clone */
        void Clone(const HComponent& c) override;

        /** @copydoc Component::Clone */
        void Clone(const HLight& c);

        /* @copydoc Component::MarkDirty */
        virtual void MarkDirty() { _internal->_markCoreDirty(); }

        /** @copydoc Component::Update */
        void Update() override { }

    protected:
        mutable SPtr<Light> _internal;

        // Only valid during construction
        LightType _type = LightType::Directional;
        Color _color = Color::White;
        float _intensity = Light::DefaultAttRadius;
        float _range = Light::DefaultAttRadius;
        float _linearAtt = Light::DefaultLinearAtt;
        float _quadraticAtt = Light::DefaultQuadraticAtt;
        bool _castShadows = Light::DefaultCastShadow;
        Degree _spotAngle = Degree(Light::DefaultSpotAngle);

    protected:
        friend class SceneObject;

        /** @copydoc Component::_instantiate */
        void _instantiate() override;

        /** @copydoc Component::OnInitialized */
        void OnInitialized() override;

        /** @copydoc Component::OnEnabled */
        void OnEnabled() override;

        /** @copydoc Component::OnEnabled */
        void OnDisabled() override;

        /** @copydoc Component::OnTransformChanged */
        void OnTransformChanged(TransformChangedFlags flags) override;

        /** @copydoc Component::OnDestroyed */
        void OnDestroyed() override;

    protected:
        CLight();
    };
}
