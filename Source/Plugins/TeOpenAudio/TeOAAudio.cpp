#include "TeOAAudio.h"
#include "TeOAAudioClip.h"
#include "Math/TeMath.h"
#include "TeOAAudioListener.h"
#include "TeOAAudioSource.h"
#include "Audio/TeAudioUtility.h"
#include "AL/al.h"

namespace te
{
    OAAudio::OAAudio()
    {
        bool enumeratedDevices;
        if (alcIsExtensionPresent(nullptr, "ALC_ENUMERATE_ALL_EXT") != ALC_FALSE)
        {
            const ALCchar* defaultDevice = alcGetString(nullptr, ALC_DEFAULT_ALL_DEVICES_SPECIFIER);
            _defaultDevice.Name = String(defaultDevice);

            const ALCchar* devices = alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER);

            Vector<char> deviceName;
            while (true)
            {
                if (*devices == 0)
                {
                    if (deviceName.empty())
                        break;

                    // Clean up the name to get the actual hardware name
                    String fixedName(deviceName.data(), deviceName.size());
                    fixedName = ReplaceAll(fixedName, u8"OpenAL Soft on ", u8"");

                    _allDevices.push_back({ fixedName });
                    deviceName.clear();

                    devices++;
                    continue;
                }

                deviceName.push_back(*devices);
                devices++;
            }

            enumeratedDevices = true;
        }
        else
        {
            _allDevices.push_back({ u8"" });
            enumeratedDevices = false;
        }

        _activeDevice = _defaultDevice;

        String defaultDeviceName = _defaultDevice.Name;
        if (enumeratedDevices)
            _device = alcOpenDevice(defaultDeviceName.c_str());
        else
            _device = alcOpenDevice(nullptr);

        if (_device == nullptr)
            TE_DEBUG("Failed to open OpenAL device: " + defaultDeviceName);

        RebuildContexts();
    }

    OAAudio::~OAAudio()
    {
        StopManualSources();

        assert(_listeners.empty() && _sources.empty()); // Everything should be destroyed at this point
        ClearContexts();

        if (_device != nullptr)
            alcCloseDevice(_device);
    }

    void OAAudio::SetVolume(float volume)
    {
        _volume = Math::Clamp01(volume);

        for (auto& listener : _listeners)
            listener->Rebuild();
    }

    float OAAudio::GetVolume() const
    {
        return _volume;
    }

    void OAAudio::SetPaused(bool paused)
    {
        if (_isPaused == paused)
            return;

        _isPaused = paused;

        for (auto& source : _sources)
            source->SetGlobalPause(paused);
    }

    void OAAudio::Update()
    {
        auto worker = [this]() { UpdateStreaming(); };

        // TODO

        Audio::Update();
    }

    void OAAudio::SetActiveDevice(const AudioDevice& device)
    {
        if (_allDevices.size() == 1)
            return; // No devices to change to, keep the active device as is

        ClearContexts();

        if (_device != nullptr)
            alcCloseDevice(_device);

        _activeDevice = device;

        String narrowName = device.Name;
        _device = alcOpenDevice(narrowName.c_str());
        if (_device == nullptr)
            TE_DEBUG("Failed to open OpenAL device: " + narrowName);

        RebuildContexts();
    }

    bool OAAudio::_isExtensionSupported(const String& extension) const
    {
        if (_device == nullptr)
            return false;

        if ((extension.length() > 2) && (extension.substr(0, 3) == "ALC"))
            return alcIsExtensionPresent(_device, extension.c_str()) != AL_FALSE;
        else
            return alIsExtensionPresent(extension.c_str()) != AL_FALSE;
    }

    void OAAudio::_registerListener(OAAudioListener* listener)
    {
        _listeners.push_back(listener);
        RebuildContexts();
    }

    void OAAudio::_unregisterListener(OAAudioListener* listener)
    {
        auto iterFind = std::find(_listeners.begin(), _listeners.end(), listener);
        if (iterFind != _listeners.end())
            _listeners.erase(iterFind);

        RebuildContexts();
    }

    void OAAudio::_registerSource(OAAudioSource* source)
    {
        _sources.insert(source);
    }

    void OAAudio::_unregisterSource(OAAudioSource* source)
    {
        _sources.erase(source);
    }

    ALCcontext* OAAudio::_getContext(const OAAudioListener* listener) const
    {
        if (_listeners.size() > 0)
        {
            assert(_listeners.size() == _contexts.size());

            UINT32 numContexts = (UINT32)_contexts.size();
            for (UINT32 i = 0; i < numContexts; i++)
            {
                if (_listeners[i] == listener)
                    return _contexts[i];
            }
        }
        else
            return _contexts[0];

        TE_DEBUG("Unable to find context for an audio listener.");
        return nullptr;
    }

    ALenum OAAudio::_getOpenALBufferFormat(UINT32 numChannels, UINT32 bitDepth)
    {
        switch (bitDepth)
        {
        case 8:
        {
            switch (numChannels)
            {
            case 1:  return AL_FORMAT_MONO8;
            case 2:  return AL_FORMAT_STEREO8;
            case 4:  return alGetEnumValue("AL_FORMAT_QUAD8");
            case 6:  return alGetEnumValue("AL_FORMAT_51CHN8");
            case 7:  return alGetEnumValue("AL_FORMAT_61CHN8");
            case 8:  return alGetEnumValue("AL_FORMAT_71CHN8");
            default:
                assert(false);
                return 0;
            }
        }
        case 16:
        {
            switch (numChannels)
            {
            case 1:  return AL_FORMAT_MONO16;
            case 2:  return AL_FORMAT_STEREO16;
            case 4:  return alGetEnumValue("AL_FORMAT_QUAD16");
            case 6:  return alGetEnumValue("AL_FORMAT_51CHN16");
            case 7:  return alGetEnumValue("AL_FORMAT_61CHN16");
            case 8:  return alGetEnumValue("AL_FORMAT_71CHN16");
            default:
                assert(false);
                return 0;
            }
        }
        case 32:
        {
            switch (numChannels)
            {
            case 1:  return alGetEnumValue("AL_FORMAT_MONO_FLOAT32");
            case 2:  return alGetEnumValue("AL_FORMAT_STEREO_FLOAT32");
            case 4:  return alGetEnumValue("AL_FORMAT_QUAD32");
            case 6:  return alGetEnumValue("AL_FORMAT_51CHN32");
            case 7:  return alGetEnumValue("AL_FORMAT_61CHN32");
            case 8:  return alGetEnumValue("AL_FORMAT_71CHN32");
            default:
                assert(false);
                return 0;
            }
        }
        default:
            assert(false);
            return 0;
        }
    }

    void OAAudio::_writeToOpenALBuffer(UINT32 bufferId, UINT8* samples, const AudioDataInfo& info)
    {
        if (info.NumChannels <= 2) // Mono or stereo
        {
            if (info.BitDepth > 16)
            {
                if (_isExtensionSupported("AL_EXT_float32"))
                {
                    UINT32 bufferSize = info.NumSamples * sizeof(float);
                    float* sampleBufferFloat = (float*)te_allocate(bufferSize);

                    AudioUtility::ConvertToFloat(samples, info.BitDepth, sampleBufferFloat, info.NumSamples);

                    ALenum format = _getOpenALBufferFormat(info.NumChannels, info.BitDepth);
                    alBufferData(bufferId, format, sampleBufferFloat, bufferSize, info.SampleRate);

                    te_delete(sampleBufferFloat);
                }
                else
                {
                    TE_DEBUG("OpenAL doesn't support bit depth larger than 16. Your audio data will be truncated.");

                    UINT32 bufferSize = info.NumSamples * 2;
                    UINT8* sampleBuffer16 = (UINT8*)te_allocate(bufferSize);

                    AudioUtility::ConvertBitDepth(samples, info.BitDepth, sampleBuffer16, 16, info.NumSamples);

                    ALenum format = _getOpenALBufferFormat(info.NumChannels, 16);
                    alBufferData(bufferId, format, sampleBuffer16, bufferSize, info.SampleRate);

                    te_delete(sampleBuffer16);
                }
            }
            else if (info.BitDepth == 8)
            {
                // OpenAL expects unsigned 8-bit data, but engine stores it as signed, so convert
                UINT32 bufferSize = info.NumSamples * (info.BitDepth / 8);
                UINT8* sampleBuffer = (UINT8*)te_allocate(bufferSize);

                for (UINT32 i = 0; i < info.NumSamples; i++)
                    sampleBuffer[i] = ((INT8*)samples)[i] + 128;

                ALenum format = _getOpenALBufferFormat(info.NumChannels, 16);
                alBufferData(bufferId, format, sampleBuffer, bufferSize, info.SampleRate);

                te_delete(sampleBuffer);
            }
            else
            {
                ALenum format = _getOpenALBufferFormat(info.NumChannels, info.BitDepth);
                alBufferData(bufferId, format, samples, info.NumSamples * (info.BitDepth / 8), info.SampleRate);
            }
        }
        else // Multichannel
        {
            // Note: Assuming AL_EXT_MCFORMATS is supported. If it's not, channels should be reduced to mono or stereo.

            if (info.BitDepth == 24) // 24-bit not supported, convert to 32-bit
            {
                UINT32 bufferSize = info.NumSamples * sizeof(INT32);
                UINT8* sampleBuffer32 = (UINT8*)te_allocate(bufferSize);

                AudioUtility::ConvertBitDepth(samples, info.BitDepth, sampleBuffer32, 32, info.NumSamples);

                ALenum format = _getOpenALBufferFormat(info.NumChannels, 32);
                alBufferData(bufferId, format, sampleBuffer32, bufferSize, info.SampleRate);

                te_delete(sampleBuffer32);
            }
            else if (info.BitDepth == 8)
            {
                // OpenAL expects unsigned 8-bit data, but engine stores it as signed, so convert
                UINT32 bufferSize = info.NumSamples * (info.BitDepth / 8);
                UINT8* sampleBuffer = (UINT8*)te_allocate(bufferSize);

                for (UINT32 i = 0; i < info.NumSamples; i++)
                    sampleBuffer[i] = ((INT8*)samples)[i] + 128;

                ALenum format = _getOpenALBufferFormat(info.NumChannels, 16);
                alBufferData(bufferId, format, sampleBuffer, bufferSize, info.SampleRate);

                te_delete(sampleBuffer);
            }
            else
            {
                ALenum format = _getOpenALBufferFormat(info.NumChannels, info.BitDepth);
                alBufferData(bufferId, format, samples, info.NumSamples * (info.BitDepth / 8), info.SampleRate);
            }
        }
    }

    void OAAudio::RebuildContexts()
    {
        for (auto& source : _sources)
            source->Clear();

        ClearContexts();

        if (_device == nullptr)
            return;

        UINT32 numListeners = (UINT32)_listeners.size();
        UINT32 numContexts = numListeners > 1 ? numListeners : 1;

        for (UINT32 i = 0; i < numContexts; i++)
        {
            ALCcontext* context = alcCreateContext(_device, nullptr);
            _contexts.push_back(context);
        }

        // If only one context is available keep it active as an optimization. Audio listeners and sources will avoid
        // excessive context switching in such case.
        alcMakeContextCurrent(_contexts[0]);

        for (auto& listener : _listeners)
            listener->Rebuild();

        for (auto& source : _sources)
            source->Rebuild();
    }

    /** Delete all existing OpenAL contexts. */
    void OAAudio::ClearContexts()
    {
        alcMakeContextCurrent(nullptr);

        for (auto& context : _contexts)
            alcDestroyContext(context);

        _contexts.clear();
    }

    /** Streams new data to audio sources that require it. */
    void OAAudio::UpdateStreaming()
    {
        {
            Lock lock(_mutex);

            for (auto& command : _streamingCommandQueue)
            {
                switch (command.type)
                {
                case StreamingCommandType::Start:
                    _streamingSources.insert(command.source);
                    break;
                case StreamingCommandType::Stop:
                    _streamingSources.erase(command.source);
                    break;
                default:
                    break;
                }
            }

            _streamingCommandQueue.clear();
            _destroyedSources.clear();
        }

        for (auto& source : _streamingSources)
        {
            // Check if the source got destroyed while streaming
            {
                Lock lock(_mutex);

                auto iterFind = _destroyedSources.find(source);
                if (iterFind != _destroyedSources.end())
                    continue;
            }

            source->Stream();
        }
    }

    SPtr<AudioClip> OAAudio::CreateClip(const SPtr<DataStream>& samples, UINT32 streamSize, UINT32 numSamples,
        const AUDIO_CLIP_DESC& desc)
    {
        return te_core_ptr_new<OAAudioClip>(samples, streamSize, numSamples, desc);
    }

    SPtr<AudioListener> OAAudio::CreateListener()
    {
        return te_core_ptr_new<OAAudioListener>();
    }

    SPtr<AudioSource> OAAudio::CreateSource()
    {
        return te_core_ptr_new<OAAudioSource>();
    }

    OAAudio& gOAAudio()
    {
        return static_cast<OAAudio&>(OAAudio::Instance());
    }
}
