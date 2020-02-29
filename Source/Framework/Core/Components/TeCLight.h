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
            float intensity = 0.5f, float range = 1.0f, bool castsShadows = false, Degree spotAngle = Degree(45));
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

        /** @copydoc Light::SetCastsShadow */
        void SetCastsShadow(bool castsShadow) { _internal->SetCastsShadow(castsShadow); }

        /** @copydoc Light::GetCastsShadow */
        bool GetCastsShadow() const { return _internal->GetCastsShadow(); }

        /** @copydoc Light::SetShadowBias */
        void SetShadowBias(float bias) { _internal->SetShadowBias(bias); }

        /** @copydoc Light::SetShadowBias() */
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
        float _intensity = 0.5f;
        float _range = 1.0f;
        float _linearAtt = 0.08f;
        float _quadraticAtt = 0.0f;
        bool _castsShadows = false;
        Degree _spotAngle = Degree(45);

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
