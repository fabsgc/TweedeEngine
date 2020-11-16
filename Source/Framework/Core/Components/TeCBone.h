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

        /** Determines the name of the bone the component is referencing. */
        void SetBoneName(const String& name);

        /** @copydoc SetBoneName */
        const String& GetBoneName() const { return _boneName; }

         /**
          * Changes the parent animation of this component.
          *
          * @param[in]	animation	New animation parent, can be null.
          * @param[in]	isInternal	If true the bone will just be changed internally, but parent animation will not be
          *							notified.
          */
        void _setParent(const HAnimation& animation, bool isInternal = false);

        /** @copydoc Component::Clone */
        void Clone(const HComponent& c) override;

        /** @copydoc Component::Clone */
        void Clone(const HBone& c);

        /** @copydoc Component::update */
        void Update() override { }

    protected:
        friend class SceneObject;

        /** Attempts to find the parent Animation component and registers itself with it. */
        void UpdateParentAnimation();

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
        void OnTransformChanged(TransformChangedFlags flags);

        /** @copydoc Component::OnDestroyed */
        void OnDestroyed() override;

    protected:
        CBone();

    protected:
        String _boneName;
        HAnimation _parent;
    };
}
