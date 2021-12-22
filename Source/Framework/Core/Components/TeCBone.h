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
        virtual ~CBone() = default;

        /** Return Component type */
        static UINT32 GetComponentType() { return TypeID_Core::TID_CBone; }

        /** @copydoc Component::Clone */
        bool Clone(const HComponent& c, const String& suffix = "") override;

        /** @copydoc Component::Clone */
        bool Clone(const HBone& c, const String& suffix = "");

        /** Determines the name of the bone the component is referencing. */
        void SetBoneName(const String& name);

        /** @copydoc SetBoneName */
        const String& GetBoneName() const { return _boneName; }

    protected:
        friend class CAnimation;

        /**
         * Changes the parent animation of this component.
         *
         * @param[in]	animation	New animation parent, can be null.
         * @param[in]	isInternal	If true the bone will just be changed internally, but parent animation will not be
         *							notified.
         */
        void SetParent(const HAnimation& animation, bool isInternal = false);

    protected:
        friend class SceneObject;

        CBone();
        CBone(const HSceneObject& parent);

        /** Attempts to find the parent Animation component and registers itself with it. */
        void UpdateParentAnimation();

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
        String _boneName;
        HAnimation _parent;
    };
}
