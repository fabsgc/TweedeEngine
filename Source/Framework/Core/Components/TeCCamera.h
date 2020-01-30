#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeCamera.h"
#include "Scene/TeComponent.h"

namespace te
{
    /**
     * @copydoc	Camera
     *
     * @note	Wraps Camera as a Component.
     */
    class TE_CORE_EXPORT CCamera : public Component
    {
    public:
        CCamera(const HSceneObject& parent);

        virtual ~CCamera();

    protected:
        mutable SPtr<Camera> _internal;

    protected:
        friend class SceneObject;

        /** @copydoc Component::OnInitialized */
        void OnInitialized() override { } 

        /** @copydoc Component::onDestroyed */
        void OnDestroyed() override { }

        /** @copydoc Component::update */
        void Update() override { }

    protected:
        CCamera();
    };
}
