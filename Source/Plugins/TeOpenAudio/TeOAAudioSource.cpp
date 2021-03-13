#include "TeOAAudioSource.h"
#include "TeOAAudio.h"
#include "TeOAAudioClip.h"
#include "AL/al.h"

namespace te
{
    OAAudioSource::OAAudioSource()
    { 
        // TODO
    }

    OAAudioSource::~OAAudioSource()
    { 
        // TODO
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
}
