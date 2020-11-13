#pragma once

#include "TeCorePrerequisites.h"
#include "Scene/TeComponent.h"

namespace te
{
    /**
     * Component that maps animation for specific bone also be applied to the SceneObject this component is attached to.
     * The component will attach to the first found parent Animation component.
     */
    class TE_CORE_EXPORT CBone : public Component
    {
    public:
        CBone(const HSceneObject& parent);
        virtual ~CBone();

        /** @copydoc Component::Initialize */
        void Initialize() override;

        /** @copydoc Component::Clone */
        void Clone(const HComponent& c) override;

        /** @copydoc Component::Clone */
        void Clone(const HBone& c);

        /** @copydoc Component::update */
        void Update() override { }

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

    protected:
        CBone();

    protected:
        String _boneName;
        HAnimation _parent;
    };
}
