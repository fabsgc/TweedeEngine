#include "Audio/TeAudioClip.h"
#include "Audio/TeAudio.h"
#include "Resources/TeResourceManager.h"

namespace te
{
    AudioClip::AudioClip(const SPtr<DataStream>& samples, UINT32 streamSize, UINT32 numSamples, const AUDIO_CLIP_DESC& desc)
        : Resource(TID_AudioClip)
        , _desc(desc)
        , _numSamples(numSamples)
        , _streamSize(streamSize)
        , _streamData(samples)
    {
        if (samples != nullptr)
            _streamOffset = (UINT32)samples->Tell();
    }

    void AudioClip::Initialize()
	{
		_length = _numSamples / _desc.NumChannels / (float)_desc.Frequency;
		Resource::Initialize();
	}

	HAudioClip AudioClip::Create(const SPtr<DataStream>& samples, UINT32 streamSize, UINT32 numSamples, const AUDIO_CLIP_DESC& desc)
	{
		return static_resource_cast<AudioClip>(gResourceManager()._createResourceHandle(_createPtr(samples, streamSize, numSamples, desc)));
	}

	SPtr<AudioClip> AudioClip::_createPtr(const SPtr<DataStream>& samples, UINT32 streamSize, UINT32 numSamples, const AUDIO_CLIP_DESC& desc)
	{
		//SPtr<AudioClip> newClip = gAudio().CreateClip(samples, streamSize, numSamples, desc);
		SPtr<AudioClip> newClip = nullptr;
		return newClip;
	}

	SPtr<AudioClip> AudioClip::CreateEmpty()
	{
		AUDIO_CLIP_DESC desc;
		//SPtr<AudioClip> newClip = gAudio().createClip(nullptr, 0, 0, desc);
		SPtr<AudioClip> newClip = nullptr;
		return newClip;
	}
}
