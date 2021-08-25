#include "Audio/TeAudio.h"
#include "Audio/TeAudioSource.h"
#include "Scene/TeTransform.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(Audio)

    void Audio::Play(const HAudioClip& clip, const Vector3& position, float volume)
    {
        Transform transform;
        transform.SetPosition(position);

        SPtr<AudioSource> source = AudioSource::Create();
        source->SetClip(clip);
        source->SetTransform(transform);
        source->SetVolume(volume);
        source->Play();

        _manualSources.push_back(source);
    }

    void Audio::StopManualSources()
    {
        for (auto& source : _manualSources)
            source->Stop();

        _manualSources.clear();
    }

    void Audio::Update()
    {
        const UINT32 numSources = (UINT32)_manualSources.size();
        for (UINT32 i = 0; i < numSources; i++)
        {
            if (_manualSources[i]->GetState() != AudioSourceState::Stopped)
                _manualSources.push_back(_manualSources[i]);
        }

        std::swap(_tempSources, _manualSources);
        _manualSources.clear();
    }

    Audio& gAudio()
    {
        return Audio::Instance();
    }
}
