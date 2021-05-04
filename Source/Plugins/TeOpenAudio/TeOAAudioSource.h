#pragma once

#include "TeOAPrerequisites.h"
#include "Audio/TeAudioSource.h"
#include "Threading/TeThreading.h"

namespace te
{
    /** OpenAL implementation of an AudioSource. */
    class OAAudioSource : public AudioSource
    {
    public:
        OAAudioSource();
        virtual ~OAAudioSource();

        /** @copydoc SceneActor::SetTransform */
        void SetTransform(const Transform& transform) override;

        /** @copydoc AudioSource::SetClip */
        void SetClip(const HAudioClip& clip) override;

        /** @copydoc AudioSource::SetVelocity */
        void SetVelocity(const Vector3& velocity) override;

        /** @copydoc AudioSource::SetVolume */
        void SetVolume(float volume) override;

        /** @copydoc AudioSource::SetPitch */
        void SetPitch(float pitch) override;

        /** @copydoc AudioSource::SetIsLooping */
        void SetIsLooping(bool loop) override;

        /** @copydoc AudioSource::SetPriority */
        void SetPriority(INT32 priority) override;

        /** @copydoc AudioSource::SetMinDistance */
        void SetMinDistance(float distance) override;

        /** @copydoc AudioSource::SetAttenuation */
        void SetAttenuation(float attenuation) override;

        /** @copydoc AudioSource::SetIsPlay3D */
        void SetIsPlay3D(bool play3D) override;

        /** @copydoc AudioSource::SetTime */
        void SetTime(float time) override;

        /** @copydoc AudioSource::GetTime */
        float GetTime() const override;

        /** @copydoc AudioSource::Play */
        void Play() override;

        /** @copydoc AudioSource::Pause */
        void Pause() override;

        /** @copydoc AudioSource::Stop */
        void Stop() override;

        /** @copydoc AudioSource::GetState */
        AudioSourceState GetState() const override;

    private:
        friend class OAAudio;

        /** Destroys the internal representation of the audio source. */
        void Clear();

        /** Rebuilds the internal representation of an audio source. */
        void Rebuild();

        /** Streams new data into the source audio buffer, if needed. */
        void Stream();

        /** Same as Stream(), but without a mutex lock (up to the caller to lock it). */
        void StreamUnlocked();

        /** Starts data streaming from the currently attached audio clip. */
        void StartStreaming();

        /** Stops streaming data from the currently attached audio clip. */
        void StopStreaming();

        /** Pauses or resumes audio playback due to the global pause setting. */
        void SetGlobalPause(bool pause);

        /** Returns true if the sound source is three dimensional (volume and pitch varies based on listener distance and velocity). */
        bool Is3D() const;

        /** Returns true if the audio source is receiving audio data from a separate thread (as opposed to loading it all at once). */
        bool RequiresStreaming() const;

        /** Fills the provided buffer with streaming data. */
        bool FillBuffer(UINT32 buffer, AudioDataInfo& info, UINT32 maxNumSamples);

        /** Makes the current audio clip active. Should be called whenever the audio clip changes. */
        void ApplyClip();

    private:
        Vector<UINT32> _sourceIDs;
        float _savedTime = 0.0f;
        AudioSourceState _savedState = AudioSourceState::Stopped;
        bool _globallyPaused = false;

        static const UINT32 _streamBufferCount = 3; // Maximum 32
        UINT32 _streamBuffers[_streamBufferCount];
        UINT32 _busyBuffers[_streamBufferCount];
        UINT32 _streamProcessedPosition = 0;
        UINT32 _streamQueuedPosition = 0;
        bool _isStreaming = false;
        mutable Mutex _mutex;
    };
}
