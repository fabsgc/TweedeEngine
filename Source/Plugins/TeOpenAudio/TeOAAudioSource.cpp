#include "TeOAAudioSource.h"
#include "TeOAAudio.h"
#include "TeOAAudioClip.h"
#include "AL/al.h"

namespace te
{
    OAAudioSource::OAAudioSource()
        // TODO
    { 
        gOAAudio()._registerSource(this);
        Rebuild();
    }

    OAAudioSource::~OAAudioSource()
    { 
        Clear();
        gOAAudio()._unregisterSource(this);
    }

    void OAAudioSource::SetTime(float time)
    {
        // TODO
    }

    float OAAudioSource::GetTime() const
    {
        return 0.0f; // TODO
    }

    void OAAudioSource::Play()
    { 
        // TODO
    }

    void OAAudioSource::Pause()
    { 
        // TODO
    }

    void OAAudioSource::Stop()
    { 
        // TODO
    }

    AudioSourceState OAAudioSource::GetState() const
    { 
        ALint state;
        alGetSourcei(_sourceIDs[0], AL_SOURCE_STATE, &state);

        switch (state)
        {
        case AL_PLAYING:
            return AudioSourceState::Playing;
        case AL_PAUSED:
            return AudioSourceState::Paused;
        case AL_INITIAL:
        case AL_STOPPED:
        default:
            return AudioSourceState::Stopped;
        }
    }

    /** Destroys the internal representation of the audio source. */
    void OAAudioSource::Clear()
    {
        // TODO
    }

    /** Rebuilds the internal representation of an audio source. */
    void OAAudioSource::Rebuild()
    {
        // TODO
    }

    /** Streams new data into the source audio buffer, if needed. */
    void OAAudioSource::Stream()
    {
        // TODO
    }

    /** Same as Stream(), but without a mutex lock (up to the caller to lock it). */
    void OAAudioSource::StreamUnlocked()
    {
        // TODO
    }

    /** Starts data streaming from the currently attached audio clip. */
    void OAAudioSource::StartStreaming()
    {
        // TODO
    }

    /** Stops streaming data from the currently attached audio clip. */
    void OAAudioSource::StopStreaming()
    {
        // TODO
    }

    /** Pauses or resumes audio playback due to the global pause setting. */
    void OAAudioSource::SetGlobalPause(bool pause)
    {
        // TODO
    }
}
