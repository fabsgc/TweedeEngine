#include "Audio/TeAudioListener.h"
#include "Audio/TeAudio.h"

namespace te
{
    AudioListener::AudioListener()
    { }

    void AudioListener::SetVelocity(const Vector3& velocity)
    {
        _velocity = velocity;
    }

    SPtr<AudioListener> AudioListener::Create()
    {
        return gAudio().CreateListener();
    }

    void AudioListener::Initialize()
    {
        CoreObject::Initialize();
    }
}
