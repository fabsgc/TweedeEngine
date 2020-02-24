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
            float intensity = 100.0f, float range = 1.0f, bool castsShadows = false, Degree spotAngle = Degree(45),
            Degree spotFalloffAngle = Degree(40));
        virtual ~CLight();

        /** @copydoc Component::Initialize */
        void Initialize() override;

        /** @copydoc Light::SetType */
        void SetType(LightType type) { _internal->SetType(type); }

        /** @copydoc Light::GetType */
        LightType GetType() const { return _internal->GetType(); }

        /** @copydoc Light::setColor */
        void SetColor(const Color& color) { _internal->SetColor(color); }

        /** @copydoc Light::getColor */
        Color GetColor() const { return _internal->GetColor(); }

        /** @copydoc Light::setIntensity */
        void SetIntensity(float intensity) { _internal->SetIntensity(intensity); }

        /** @copydoc Light::getIntensity */
        float GetIntensity() const { return _internal->GetIntensity(); }

        /**  @copydoc Light::setUseAutoAttenuation */
        void SetUseAutoAttenuation(bool enabled) { _internal->SetUseAutoAttenuation(enabled); }

        /** @copydoc Light::getUseAutoAttenuation */
        bool GetUseAutoAttenuation() const { return _internal->GetUseAutoAttenuation(); }

        /** @copydoc Light::setAttenuationRadius */
        void SetAttenuationRadius(float radius) { _internal->SetAttenuationRadius(radius); }

        /** @copydoc Light::getAttenuationRadius */
        float GetAttenuationRadius() const { return _internal->GetAttenuationRadius(); }

        /** @copydoc Light::setSourceRadius */
        void SetSourceRadius(float radius) { _internal->SetSourceRadius(radius); }

        /** @copydoc Light::getSourceRadius */
        float GetSourceRadius() const { return _internal->GetSourceRadius(); }

        /** @copydoc Light::setSpotAngle */
        void SetSpotAngle(const Degree& spotAngle) { _internal->SetSpotAngle(spotAngle); }

        /** @copydoc Light::getSpotAngle */
        Degree GetSpotAngle() const { return _internal->GetSpotAngle(); }

        /** @copydoc Light::setSpotFalloffAngle */
        void SetSpotFalloffAngle(const Degree& spotAngle) { _internal->SetSpotFalloffAngle(spotAngle); }

        /** @copydoc Light::getSpotFalloffAngle */
        Degree GetSpotFalloffAngle() const { return _internal->GetSpotFalloffAngle(); }

        /** @copydoc Light::setCastsShadow */
        void SetCastsShadow(bool castsShadow) { _internal->SetCastsShadow(castsShadow); }

        /** @copydoc Light::getCastsShadow */
        bool GetCastsShadow() const { return _internal->GetCastsShadow(); }

        /** @copydoc Light::setShadowBias */
        void SetShadowBias(float bias) { _internal->SetShadowBias(bias); }

        /** @copydoc Light::setShadowBias() */
        float GetShadowBias() const { return _internal->GetShadowBias(); }

        /** @copydoc Light::GetBounds */
        Sphere GetBounds() const;

        /** Returns the light that this component wraps. */
        SPtr<Light> _getLight() const { return _internal; }

        static UINT32 GetComponentType() { return TID_CLight; }

    protected:
        mutable SPtr<Light> _internal;

        // Only valid during construction
        LightType _type = LightType::Directional;
        Color _color = Color::White;
        float _intensity = 100.0f;
        float _range = 1.0f;
        bool _castsShadows = false;
        Degree _spotAngle = Degree(45);
        Degree _spotFalloffAngle = Degree(40);

    protected:
        friend class SceneObject;

        /** @copydoc Component::_instantiate */
        void _instantiate() override;

        /** @copydoc Component::OnInitialized */
        void OnCreated() override;

        /** @copydoc Component::OnInitialized */
        void OnInitialized() override;

        /** @copydoc Component::OnTransformChanged */
        void OnTransformChanged(TransformChangedFlags flags) override;

        /** @copydoc Component::OnDestroyed */
        void OnDestroyed() override;

    public:
        /** @copydoc Component::Update */
        void Update() override { }
    
    protected:
        CLight();
    };
}
