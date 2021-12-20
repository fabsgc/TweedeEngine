#include "Audio/TeAudioSource.h"
#include "Audio/TeAudio.h"
#include "Math/TeMath.h"

namespace te
{
    AudioSource::AudioSource()
    { }

    void AudioSource::SetClip(const HAudioClip& clip)
    {
        _audioClip = clip;
    }

    void AudioSource::SetVelocity(const Vector3& velocity)
    {
        _velocity = velocity;
    }

    void AudioSource::SetVolume(float volume)
    {
        _volume = Math::Clamp01(volume);
    }

    void AudioSource::SetPitch(float pitch)
    {
        _pitch = pitch;
    }

    void AudioSource::SetIsLooping(bool loop)
    {
        _loop = loop;
    }

    void AudioSource::SetPriority(INT32 priority)
    {
        _priority = priority;
    }

    void AudioSource::SetMinDistance(float distance)
    {
        _minDistance = distance;
    }

    void AudioSource::SetAttenuation(float attenuation)
    {
        _attenuation = attenuation;
    }

    void AudioSource::SetIsPlay3D(bool play3D)
    {
        _play3D = play3D;
    }

    void AudioSource::_markCoreDirty(ActorDirtyFlag flag)
    {
        MarkCoreDirty((UINT32)flag);
    }

    SPtr<AudioSource> AudioSource::Create()
    {
        SPtr<AudioSource> source = gAudio().CreateSource();
        source->SetThisPtr(source);
        source->Initialize();
        
        return source;
    }

    void AudioSource::Initialize()
    {
        CoreObject::Initialize();
    }
}
