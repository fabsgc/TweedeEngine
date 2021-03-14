#pragma once

#include "TeOAPrerequisites.h"
#include "Audio/TeAudioSource.h"

namespace te
{
    /** OpenAL implementation of an AudioSource. */
    class OAAudioSource : public AudioSource
    {
    public:
        OAAudioSource();
        virtual ~OAAudioSource();

        // TODO

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

    private:
        Vector<UINT32> _sourceIDs;
    };
}
