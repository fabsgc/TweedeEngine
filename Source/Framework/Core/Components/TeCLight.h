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
        CLight(const HSceneObject& parent);

        virtual ~CLight();

    protected:
        mutable SPtr<Light> _internal;

    protected:
        friend class SceneObject;

        /** @copydoc Component::OnInitialized */
        void OnInitialized() override { }

        /** @copydoc Component::onDestroyed */
        void OnDestroyed() override { }

        /** @copydoc Component::update */
        void Update() override { }

    protected:
        CLight();
    };
}
