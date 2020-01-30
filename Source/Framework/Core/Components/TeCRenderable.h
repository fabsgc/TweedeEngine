#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeRenderable.h"
#include "Scene/TeComponent.h"

namespace te
{
    /**
     * @copydoc	Renderable
     *
     * @note	Wraps Renderable as a Component.
     */
    class TE_CORE_EXPORT CRenderable : public Component
    {
    public:
        CRenderable(const HSceneObject& parent);

        virtual ~CRenderable();

    protected:
        mutable SPtr<Renderable> _internal;

    protected:
        friend class SceneObject;

        /** @copydoc Component::OnInitialized */
        void OnInitialized() override;

        /** @copydoc Component::onDestroyed */
        void OnDestroyed() override;

        /** @copydoc Component::update */
        void Update() override { }

    protected:
        CRenderable();
    };
}
