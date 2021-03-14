#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"
#include "Math/TeVector3.h"
#include "Audio/TeAudioClip.h"

namespace te
{
    /** Identifier for a device that can be used for playing audio. */
    struct TE_CORE_EXPORT AudioDevice
    {
        String Name;
    };

    /** Provides global functionality relating to sounds and music. */
    class TE_CORE_EXPORT Audio : public Module<Audio>
    {
    public:
        TE_MODULE_STATIC_HEADER_MEMBER(Audio)

        virtual ~Audio() = default;

        /**
         * Starts playback of the provided audio clip. This can be used for a quicker way of creating audio sources if you
         * don't need the full control provided by creating AudioSource manually.
         *
         * @param[in]	clip		Audio clip to play.
         * @param[in]	position	Position in world space to play the clip at. Only relevant if the clip is 3D.
         * @param[in]	volume		Volume to play the clip at.
         */
        void Play(const HAudioClip& clip, const Vector3& position = Vector3::ZERO, float volume = 1.0f);

        /** Determines global audio volume. In range [0, 1]. */
        virtual void SetVolume(float volume) = 0;

        /** @copydoc setVolume() */
        virtual float GetVolume() const = 0;

        /** Determines if audio reproduction is paused globally. */
        virtual void SetPaused(bool paused) = 0;

        /** @copydoc setPaused() */
        virtual bool IsPaused() const = 0;

        /** Determines the device on which is the audio played back on. */
        virtual void SetActiveDevice(const AudioDevice& device) = 0;

        /** @copydoc SetActiveDevice() */
        virtual AudioDevice GetActiveDevice() const = 0;

        /** Returns the default audio device identifier. */
        virtual AudioDevice GetDefaultDevice() const = 0;

        /** Returns a list of all available audio devices. */
        virtual const Vector<AudioDevice>& GetAllDevices() const = 0;

        /** Called once per frame. Queues streaming audio requests. */
        virtual void Update();

    protected:
        friend class AudioClip;
        friend class AudioListener;
        friend class AudioSource;

        /**
         * Creates a new audio clip.
         *
         * @param[in]	samples		Stream containing audio samples in format specified in @p desc.
         * @param[in]	streamSize	Size of the audio data in the provided stream, in bytes.
         * @param[in]	numSamples	Number of samples in @p samples stream.
         * @param[in]	desc		Descriptor describing the type of the audio stream (format, sample rate, etc.).
         * @return					Newly created AudioClip. Must be manually initialized.
         */
        virtual SPtr<AudioClip> CreateClip(const SPtr<DataStream>& samples, UINT32 streamSize, UINT32 numSamples,
            const AUDIO_CLIP_DESC& desc) = 0;

        /** Creates a new AudioListener. */
        virtual SPtr<AudioListener> CreateListener() = 0;

        /** Creates a new AudioSource. */
        virtual SPtr<AudioSource> CreateSource() = 0;

        /** Stops playback of all sources started with Audio::Play calls. */
        void StopManualSources();

    private:
        Vector<SPtr<AudioSource>> _manualSources;
        Vector<SPtr<AudioSource>> _tempSources;
    };

    /** Provides easier access to Audio. */
    TE_CORE_EXPORT Audio& gAudio();
}
