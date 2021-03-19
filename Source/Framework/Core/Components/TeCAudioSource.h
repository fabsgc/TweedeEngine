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

        static UINT32 GetComponentType() { return TID_CAudioSource; }

        /** @copydoc Component::Initialize */
        void Initialize() override;

        /** @copydoc Component::Clone */
        void Clone(const HComponent& c) override;

        /** @copydoc Component::Clone */
        void Clone(const HAudioSource& c);

        /* @copydoc Component::MarkDirty */
        virtual void MarkDirty() { _internal->MarkCoreDirty(); }

        /** @copydoc Component::Update */
        void Update() override;

        /** @copydoc AudioSource::SetClip */
        void SetClip(const HAudioClip& clip);

        /** @copydoc AudioSource::GetClip */
        HAudioClip GetClip() const { return _audioClip; }

        /** @copydoc AudioSource::SetVolume */
        void SetVolume(float volume);

        /** @copydoc AudioSource::GetVolume */
        float GetVolume() const { return _volume; }

        /** @copydoc AudioSource::SetPitch */
        void SetPitch(float pitch);

        /** @copydoc AudioSource::GetPitch */
        float GetPitch() const { return _pitch; }

        /** @copydoc AudioSource::SetIsLooping */
        void SetIsLooping(bool loop);

        /** @copydoc AudioSource::GetIsLooping */
        bool GetIsLooping() const { return _loop; }

        /** @copydoc AudioSource::SetPriority */
        void SetPriority(UINT32 priority);

        /** @copydoc AudioSource::GetPriority */
        UINT32 GetPriority() const { return _priority; }

        /** @copydoc AudioSource::SetMinDistance */
        void SetMinDistance(float distance);

        /** @copydoc AudioSource::GetMinDistance */
        float GetMinDistance() const { return _minDistance; }

        /** @copydoc AudioSource::SetAttenuation */
        void SetAttenuation(float attenuation);

        /** @copydoc AudioSource::GetAttenuation */
        float GetAttenuation() const { return _attenuation; }

        /** @copydoc AudioSource::SetTime */
        void SetTime(float time);

        /** @copydoc AudioSource::GetTime */
        float GetTime() const;

        /** Sets whether playback should start as soon as the component is enabled. */
        void SetPlayOnStart(bool enable) { _playOnStart = enable; }

        /** Determines should playback start as soon as the component is enabled. */
        bool GetPlayOnStart() const { return _playOnStart; }

        /** @copydoc AudioSource::Play */
        void Play();

        /** @copydoc AudioSource::Pause */
        void Pause();

        /** @copydoc AudioSource::Stop */
        void Stop();

        /** @copydoc AudioSource::GetState */
        AudioSourceState GetState() const;

        /** Returns the AudioSource implementation wrapped by this component. */
        AudioSource* _getInternal() const { return _internal.get(); }

        /** @copydoc AudioSource::GetTransform */
        const Transform& GetTransform() { return _internal->GetTransform(); }

        /** @copydoc SceneActor::GetActive */
        bool GetActive() const { return _internal != nullptr; }

    protected:
        friend class SceneObject;

        /** @copydoc Component::_instantiate */
        void _instantiate() override {};

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
        float _volume = 1.0f;
        float _pitch = 1.0f;
        bool _loop = false;
        UINT32 _priority = 0;
        float _minDistance = 1.0f;
        float _attenuation = 1.0f;
        bool _playOnStart = true;
    };
}
