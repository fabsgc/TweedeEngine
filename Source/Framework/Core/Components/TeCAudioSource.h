#pragma once

#include "TeCorePrerequisites.h"
#include "Scene/TeComponent.h"

namespace te
{
    /**
     * @copydoc	AudioSource
     *
     * @note Wraps AudioSource as a Component.
     */
    class TE_CORE_EXPORT CAudioSource : public Component
    {
    public:
        CAudioSource(const HSceneObject& parent);

        virtual ~CAudioSource() = default;

        static UINT32 GetComponentType() { return TID_CAudioSource; }

        /** @copydoc Component::Clone */
        void Clone(const HComponent& c) override;

        /** @copydoc Component::Clone */
        void Clone(const HAudioSource& c);

        /* @copydoc Component::MarkDirty */
        virtual void MarkDirty() { /* TODO */ }

        /** @copydoc Component::Update */
        void Update() override { }

    protected:
        HAudioClip mAudioClip;

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
        using Component::DestroyInternal;

        /**
        * Creates the internal representation of the Animation and restores the values saved by the Component.
        * Node : clips must be added manually because this method can't do it properly
        */
        void RestoreInternal();

        /** Destroys the internal Animation representation. */
        void DestroyInternal();

    protected:
        CAudioSource();
    };
}
