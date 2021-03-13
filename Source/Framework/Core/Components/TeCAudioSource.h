#pragma once

#include "TeCorePrerequisites.h"
#include "Audio/TeAudioSource.h"
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

        /** @copydoc AudioSource::setClip */
        void SetClip(const HAudioClip& clip);

        /** @copydoc AudioSource::getClip */
        HAudioClip getClip() const { return _audioClip; }

        static UINT32 GetComponentType() { return TID_CAudioSource; }

        /** @copydoc Component::Clone */
        void Clone(const HComponent& c) override;

        /** @copydoc Component::Clone */
        void Clone(const HAudioSource& c);

        /* @copydoc Component::MarkDirty */
        virtual void MarkDirty() { /* TODO */ }

        /** @copydoc Component::Update */
        void Update() override;

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

        /** Returns the AudioSource implementation wrapped by this component. */
        AudioSource* _getInternal() const { return _internal.get(); }

    protected:
        using Component::DestroyInternal;

        /**
        * Creates the internal representation of the Animation and restores the values saved by the Component.
        * Node : clips must be added manually because this method can't do it properly
        */
        void RestoreInternal();

        /** Destroys the internal Animation representation. */
        void DestroyInternal();

        /**
         * Updates the transform of the internal AudioSource representation from the transform of the component's scene
         * object.
         */
        void UpdateTransform();

    protected:
        CAudioSource();

    protected:
        SPtr<AudioSource> _internal;
        Vector3 _lastPosition = Vector3::ZERO;
        Vector3 _velocity = Vector3::ZERO;

        HAudioClip _audioClip;
    };
}
