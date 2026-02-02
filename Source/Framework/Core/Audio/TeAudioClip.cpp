#include "Audio/TeAudioClip.h"
#include "Audio/TeAudio.h"
#include "Utility/TeDataStream.h"
#include "Resources/TeResourceManager.h"

namespace te
{
    AudioClip::AudioClip(const SPtr<DataStream>& samples, UINT32 streamSize, UINT32 numSamples, const AUDIO_CLIP_DESC& desc)
        : Resource(CoreType::TID_AudioClip)
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
        Resource::Initialize();
        _length = _numSamples / _desc.NumChannels / (float)_desc.Frequency;
    }

    HAudioClip AudioClip::Create(const SPtr<DataStream>& samples, UINT32 streamSize, UINT32 numSamples, const AUDIO_CLIP_DESC& desc)
    {
        return static_resource_cast<AudioClip>(gResourceManager()._createResourceHandle(CreatePtr(samples, streamSize, numSamples, desc)));
    }

    SPtr<AudioClip> AudioClip::CreatePtr(const SPtr<DataStream>& samples, UINT32 streamSize, UINT32 numSamples, const AUDIO_CLIP_DESC& desc)
    {
        SPtr<AudioClip> newClip = gAudio().CreateClip(samples, streamSize, numSamples, desc);
        newClip->SetThisPtr(newClip);
        newClip->Initialize();

        return newClip;
    }

    SPtr<AudioClip> AudioClip::CreateEmpty()
    {
        AUDIO_CLIP_DESC desc;
        SPtr<AudioClip> newClip = gAudio().CreateClip(nullptr, 0, 0, desc);
        newClip->SetThisPtr(newClip);

        return newClip;
    }

    void AudioClip::Serialize(StreamWriter* serializer) const
    {
        Resource::Serialize(serializer);

        uint32_t size = 0;
        auto stream = GetSourceStream(size);
        uint8_t* buffer = te_allocate<uint8_t>(size);

        nlohmann::json document;

        document["numSamples"] = _numSamples;
        document["streamSize"] = size;
        document["streamOffset"] = _streamOffset;
        document["desc"]["format"] = static_cast<uint32_t>(_desc.Format);
        document["desc"]["bitDepth"] = _desc.BitDepth;
        document["desc"]["frequency"] = _desc.Frequency;
        document["desc"]["numChannels"] = _desc.NumChannels;
        document["desc"]["readMode"] = static_cast<uint32_t>(_desc.ReadMode);
        document["desc"]["is3D"] = _desc.Is3D;

        String dump = document.dump();
        serializer->WriteString(dump);

        stream->Read(buffer, size);
        serializer->WriteBuffer(buffer, size);

        te_free(buffer);
    }

    bool AudioClip::Deserialize(StreamReader* deserializer, AudioClip* object)
    {
        if (!object)
            return false;

        Resource::Deserialize(deserializer, object);

        String dump;
        deserializer->ReadString(dump);
        nlohmann::json document = nlohmann::json::parse(dump);

        object->_numSamples = document["numSamples"].get<UINT32>();
        object->_streamSize = document["streamSize"].get<UINT32>(); 
        object->_streamOffset = document["streamOffset"].get<UINT32>();
        object->_desc.Format = static_cast<AudioFormat>(document["desc"]["format"].get<uint32_t>());
        object->_desc.BitDepth = document["desc"]["bitDepth"].get<UINT32>();
        object->_desc.Frequency = document["desc"]["frequency"].get<UINT32>();
        object->_desc.NumChannels = document["desc"]["numChannels"].get<UINT32>();
        object->_desc.ReadMode = static_cast<AudioReadMode>(document["desc"]["readMode"].get<uint32_t>());
        object->_desc.Is3D = document["desc"]["is3D"].get<bool>();

        // Read sample data buffer and store in an owning memory stream
        {
            uint8_t* buffer = nullptr;
            deserializer->ReadBuffer(&buffer, 0);

            // Create an owning memory stream and copy data into it
            SPtr<MemoryDataStream> memStream = te_shared_ptr_new<MemoryDataStream>(object->_streamSize);
            if (object->_streamSize > 0)
                memStream->Write(buffer, object->_streamSize);

            memStream->Seek(0);
            object->_streamData = memStream;

            te_deallocate(buffer);
        }

        object->Initialize();

        return true;
    }
}
