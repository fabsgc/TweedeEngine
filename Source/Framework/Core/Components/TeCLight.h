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
        CLight(const HSceneObject& parent, LightType type = LightType::Directional);
        virtual ~CLight();

        /** @copydoc Component::Initialize */
        void Initialize() override;

        /** @copydoc Light::SetType */
        void setType(LightType type) { _internal->SetType(type); }

        /** @copydoc Light::GetType */
        LightType getType() const { return _internal->GetType(); }

        /** @copydoc Light::GetBounds */
        Sphere GetBounds() const;

        /** Returns the light that this component wraps. */
        SPtr<Light> _getLight() const { return _internal; }

        static UINT32 GetComponentType() { return TID_CLight; }

    protected:
        mutable SPtr<Light> _internal;

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

    protected:
        // Only valid during construction
        LightType _type = LightType::Directional;
    };
}
