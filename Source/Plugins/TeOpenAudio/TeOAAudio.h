#pragma once

#include "TeOAPrerequisites.h"
#include "Audio/TeAudio.h"
#include <AL/alc.h>
#include "TeOAAudioSource.h"

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

        /** Registers a new AudioListener. Should be called on listener creation. */
        void _registerListener(OAAudioListener* listener);

        /** Unregisters an existing AudioListener. Should be called before listener destruction. */
        void _unregisterListener(OAAudioListener* listener);

        /** Registers a new AudioSource. Should be called on source creation. */
        void _registerSource(OAAudioSource* source);

        /** Unregisters an existing AudioSource. Should be called before source destruction. */
        void _unregisterSource(OAAudioSource* source);

        /** Returns a list of all OpenAL contexts. Each listener has its own context. */
        const Vector<ALCcontext*>& _getContexts() const { return _contexts; }

        /** Returns an OpenAL context assigned to the provided listener. */
        ALCcontext* _getContext(const OAAudioListener* listener) const;

        /**
         * Returns optimal format for the provided number of channels and bit depth. It is assumed the user has checked if
         * extensions providing these formats are actually available.
         */
        INT32 _getOpenALBufferFormat(UINT32 numChannels, UINT32 bitDepth);

        /**
         * Writes provided samples into the OpenAL buffer with the provided ID. If the provided format is not supported the
         * samples will first be converted into a valid format.
         */
        void _writeToOpenALBuffer(UINT32 bufferId, UINT8* samples, const AudioDataInfo& info);

    private:
        friend class OAAudioSource;

        /** Type of a command that can be queued for a streaming audio source. */
        enum class StreamingCommandType
        {
            Start,
            Stop
        };

        /** Command queued for a streaming audio source. */
        struct StreamingCommand
        {
            StreamingCommandType type;
            OAAudioSource* source;
        };

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

        Vector<OAAudioListener*> _listeners;
        Vector<ALCcontext*> _contexts;
        UnorderedSet<OAAudioSource*> _sources;

        // Streaming thread
        Vector<StreamingCommand> _streamingCommandQueue;
        UnorderedSet<OAAudioSource*> _streamingSources;
        UnorderedSet<OAAudioSource*> _destroyedSources;
        // SPtr<Task> mStreamingTask; TODO
        mutable Mutex _mutex;
    };

    /** Provides easier access to OAAudio. */
    OAAudio& gOAAudio();
}
