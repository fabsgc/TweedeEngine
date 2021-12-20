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

    void AudioListener::_markCoreDirty(ActorDirtyFlag flag)
    {
        MarkCoreDirty((UINT32)flag);
    }

    SPtr<AudioListener> AudioListener::Create()
    {
        SPtr<AudioListener> listener = gAudio().CreateListener();
        listener->SetThisPtr(listener);
        listener->Initialize();

        return listener;
    }

    void AudioListener::Initialize()
    {
        CoreObject::Initialize();
    }
}
