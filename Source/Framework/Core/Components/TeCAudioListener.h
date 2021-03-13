#pragma once

#include "TeCorePrerequisites.h"
#include "Scene/TeComponent.h"

namespace te
{
    /**
     * @copydoc	AudioListener
     *
     * @note Wraps AudioListener as a Component.
     */
    class TE_CORE_EXPORT CAudioListener : public Component
    {
    public:
        CAudioListener(const HSceneObject& parent);

        virtual ~CAudioListener() = default;

        static UINT32 GetComponentType() { return TID_CAudioListener; }

        /** @copydoc Component::Clone */
        void Clone(const HComponent& c) override;

        /** @copydoc Component::Clone */
        void Clone(const HAudioListener& c);

        /* @copydoc Component::MarkDirty */
        virtual void MarkDirty() { /* TODO */ }

        /** @copydoc Component::Update */
        void Update() override { }

        /** Returns the AudioListener implementation wrapped by this component. */
        AudioListener* _getInternal() const { return _internal.get(); }

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
        CAudioListener();

    protected:
        SPtr<AudioListener> _internal;
        HAudioClip _audioClip;
    };
}
