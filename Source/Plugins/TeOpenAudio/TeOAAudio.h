#pragma once

#include "TeOAPrerequisites.h"
#include "Audio/TeAudio.h"
#include <AL/alc.h>

namespace te
{
    /** Global manager for the audio implementation using OpenAL as the backend. */
    class OAAudio : public Audio
    {
    public:
        OAAudio();
        virtual ~OAAudio();

        /** @copydoc Audio::SetVolume */
        void SetVolume(float volume) override;

        /** @copydoc Audio::GetVolume */
        float GetVolume() const override;

        /** @copydoc Audio::SetPaused */
        void SetPaused(bool paused) override;

        /** @copydoc Audio::IsPaused */
        bool IsPaused() const override { return _isPaused; }

        /** @copydoc Audio::Update */
        void Update() override;

        /** @copydoc Audio::SetActiveDevice */
        void SetActiveDevice(const AudioDevice& device) override;

        /** @copydoc Audio::GetActiveDevice */
        AudioDevice GetActiveDevice() const override { return _activeDevice; }

        /** @copydoc Audio::GetDefaultDevice */
        AudioDevice GetDefaultDevice() const override { return _defaultDevice; }

        /** @copydoc Audio::GetAllDevices */
        const Vector<AudioDevice>& GetAllDevices() const override { return _allDevices; };

        /** Checks is a specific OpenAL extension supported. */
        bool _isExtensionSupported(const String& extension) const;

    private:
        /** @copydoc Audio::CreateClip */
        SPtr<AudioClip> CreateClip(const SPtr<DataStream>& samples, UINT32 streamSize, UINT32 numSamples,
            const AUDIO_CLIP_DESC& desc) override;

        /** @copydoc Audio::CreateListener */
        SPtr<AudioListener> CreateListener() override;

        /** @copydoc Audio::CreateSource */
        SPtr<AudioSource> CreateSource() override;

        /**
         * Delete all existing contexts and rebuild them according to the listener list. All audio sources will be rebuilt
         * as well.
         *
         * This should be called when listener count changes, or audio device is changed.
         */
        void RebuildContexts();

        /** Delete all existing OpenAL contexts. */
        void ClearContexts();

        /** Streams new data to audio sources that require it. */
        void UpdateStreaming();

    private:
        float _volume = 1.0f;
        bool _isPaused = false;

        ALCdevice* _device = nullptr;
        Vector<AudioDevice> _allDevices;
        AudioDevice _defaultDevice;
        AudioDevice _activeDevice;
    };
}
