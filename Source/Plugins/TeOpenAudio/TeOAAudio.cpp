#include "TeOAAudio.h"
#include "TeOAAudioClip.h"
#include "Math/TeMath.h"
#include "TeOAAudioListener.h"
#include "TeOAAudioSource.h"
#include "Audio/TeAudioUtility.h"
#include "AL/al.h"
#include "AL/alext.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(OAAudio)

    OAAudio::OAAudio()
    {
        FindAllAvailableDevices();
        SetActiveDevice(_defaultDevice);
    }

    OAAudio::~OAAudio()
    {
        StopManualSources();

        assert(_listeners.empty() && _sources.empty()); // Everything should be destroyed at this point
        
        if (_device != nullptr)
            alcCloseDevice(_device);

        ClearContexts();
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

    void OAAudio::OnDeviceAdded()
    {
        _needDeviceRefresh = true;
    }

    void OAAudio::OnDeviceRemoved()
    {
        _needDeviceRefresh = true;
    }

    void OAAudio::OnDefaultDeviceChanged()
    {
        _needDeviceRefresh = true;
    }

    void OAAudio::OnDeviceStateChanged()
    {
        _needDeviceRefresh = true;
    }

    void OAAudio::Update()
    {
        if (_needDeviceRefresh)
        {
            UpdateDevices();
            _needDeviceRefresh = false;
        }

        auto worker = [this]() { UpdateStreaming(); };

        // If previous task still hasn't completed, just skip streaming this frame, queuing more tasks won't help
        if (_streamingTask && !_streamingTask->IsComplete())
            return;

        _streamingTask = !_streamingTask ? Task::Create("AudioStream", worker) : _streamingTask;
        _streamingTask->Reset();

        gTaskScheduler().AddTask(_streamingTask);

        Audio::Update();
    }

    void OAAudio::UpdateDevices()
    {
        FindAllAvailableDevices();

        int connected = false;
        alcGetIntegerv(_device, ALC_CONNECTED, 1, &connected);
        if (!connected)
        {
            TE_DEBUG("Device disconnected");
        }

        // If active device is not present in new available devices, we switch to the default one
        if (std::find(_allDevices.begin(), _allDevices.end(), _activeDevice) == _allDevices.end() || !connected)
        {
            SetActiveDevice(_defaultDevice);
        }
    }

    void OAAudio::FindAllAvailableDevices()
    {
        _allDevices.clear();

        if (alcIsExtensionPresent(nullptr, "ALC_ENUMERATE_ALL_EXT") != ALC_FALSE)
        {
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
                    fixedName = ReplaceAll(fixedName, "OpenAL Soft on ", "");

                    _allDevices.push_back({ fixedName });
                    deviceName.clear();

                    devices++;
                    continue;
                }

                deviceName.push_back(*devices);
                devices++;
            }

            const ALCchar* defaultDevice = alcGetString(nullptr, ALC_DEFAULT_ALL_DEVICES_SPECIFIER);
            _defaultDevice.Name = ReplaceAll(String(defaultDevice), "OpenAL Soft on ", "");
        }
        else
        {
            _defaultDevice.Name = "";
            _allDevices.push_back({ "" });
        }
    }

    bool OAAudio::OpenDevice(const AudioDevice& device)
    {
        alGetError();

        ALenum error;
        String narrowName = device.Name != "" ? "OpenAL Soft on " + device.Name : "";

        _device = alcOpenDevice(narrowName != "" ? narrowName.c_str() : nullptr);

        if (_device == nullptr)
        {
            TE_DEBUG("Failed to open OpenAL device : " + narrowName);
        }
        else if ((error = alGetError()) != AL_NO_ERROR && error != AL_INVALID_OPERATION)
        {
            TE_DEBUG("Something wrong happened during device reopenning : " + narrowName);
        }
        else
        {
            int connected = false;
            alcGetIntegerv(_device, ALC_CONNECTED, 1, &connected);
            if (connected)
            {
                _activeDevice = device;
                return true;
            }
            else
            {
                TE_DEBUG("Device disconnected : " + narrowName);
            }
        }

        return false;
    }

    bool OAAudio::ReopenDevice(const AudioDevice& device)
    {
        alGetError();

        if (!_device)
        {
            TE_DEBUG("No device already opened");
            return false;
        }

        if (!alcIsExtensionPresent(_device, "ALC_SOFT_reopen_device"))
        {
            TE_DEBUG("ALC_SOFT_reopen_device not available");
            return false;
        }

        ALCboolean(ALC_APIENTRY * alcReopenDeviceSOFT)(ALCdevice * device, const ALCchar * name, const ALCint * attribs);
        alcReopenDeviceSOFT = reinterpret_cast<ALCboolean(ALC_APIENTRY*)(ALCdevice * device, const ALCchar * name, const ALCint * attribs)>(alcGetProcAddress(_device, "alcReopenDeviceSOFT"));

        String narrowName = device.Name != "" ? "OpenAL Soft on " + device.Name : "";
        if (alcReopenDeviceSOFT(_device, narrowName != "" ? narrowName.c_str() : nullptr, NULL))
        {
            ALenum error;
            if ((error = alGetError()) != AL_NO_ERROR && error != AL_INVALID_OPERATION)
            {
                TE_DEBUG("Something wrong happened during device reopenning : " + narrowName);
            }
            else
            {
                int connected = false;
                alcGetIntegerv(_device, ALC_CONNECTED, 1, &connected);
                if (connected)
                {
                    _activeDevice = device;
                    return true;
                }
                else
                {
                    TE_DEBUG("Device disconnected : " + narrowName);
                }
            }
        }
        else
        {
            TE_DEBUG("Failed to reopen OpenAL device with : " + narrowName);
        }

        return false;
    }

    void OAAudio::SetActiveDevice(const AudioDevice& device)
    {
        FindAllAvailableDevices();

        if (!_device)
        {
            if (!OpenDevice(device))
            {
                for (const auto& audioDevice : _allDevices)
                {
                    if (OpenDevice(audioDevice))
                    {
                        break;
                    }
                }
            }

            RebuildContexts();
        }
        else
        {
            if (!ReopenDevice(device))
            {
                for (const auto& audioDevice : _allDevices)
                {
                    if (ReopenDevice(audioDevice))
                    {
                        break;
                    }
                }
            }
        }
    }

    bool OAAudio::IsExtensionSupported(const String& extension) const
    {
        if (_device == nullptr)
            return false;

        if ((extension.length() > 2) && (extension.substr(0, 3) == "ALC"))
            return alcIsExtensionPresent(_device, extension.c_str()) != AL_FALSE;
        else
            return alIsExtensionPresent(extension.c_str()) != AL_FALSE;
    }

    void OAAudio::RegisterListener(OAAudioListener* listener)
    {
        _listeners.push_back(listener);
        RebuildContexts();
    }

    void OAAudio::UnregisterListener(OAAudioListener* listener)
    {
        auto iterFind = std::find(_listeners.begin(), _listeners.end(), listener);
        if (iterFind != _listeners.end())
            _listeners.erase(iterFind);

        RebuildContexts();
    }

    void OAAudio::RegisterSource(OAAudioSource* source)
    {
        _sources.insert(source);
    }

    void OAAudio::UnregisterSource(OAAudioSource* source)
    {
        _sources.erase(source);
    }

    ALCcontext* OAAudio::GetContext(const OAAudioListener* listener) const
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

    ALenum OAAudio::GetOpenALBufferFormat(UINT32 numChannels, UINT32 bitDepth)
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

    void OAAudio::WriteToOpenALBuffer(UINT32 bufferId, UINT8* samples, const AudioDataInfo& info)
    {
        if (info.NumChannels <= 2) // Mono or stereo
        {
            if (info.BitDepth > 16)
            {
                if (IsExtensionSupported("AL_EXT_float32"))
                {
                    UINT32 bufferSize = info.NumSamples * sizeof(float);
                    float* sampleBufferFloat = (float*)te_allocate(bufferSize);

                    AudioUtility::ConvertToFloat(samples, info.BitDepth, sampleBufferFloat, info.NumSamples);

                    ALenum format = GetOpenALBufferFormat(info.NumChannels, info.BitDepth);
                    alBufferData(bufferId, format, sampleBufferFloat, bufferSize, info.SampleRate);

                    te_delete(sampleBufferFloat);
                }
                else
                {
                    TE_DEBUG("OpenAL doesn't support bit depth larger than 16. Your audio data will be truncated.");

                    UINT32 bufferSize = info.NumSamples * 2;
                    UINT8* sampleBuffer16 = (UINT8*)te_allocate(bufferSize);

                    AudioUtility::ConvertBitDepth(samples, info.BitDepth, sampleBuffer16, 16, info.NumSamples);

                    ALenum format = GetOpenALBufferFormat(info.NumChannels, 16);
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

                ALenum format = GetOpenALBufferFormat(info.NumChannels, 16);
                alBufferData(bufferId, format, sampleBuffer, bufferSize, info.SampleRate);

                te_delete(sampleBuffer);
            }
            else
            {
                ALenum format = GetOpenALBufferFormat(info.NumChannels, info.BitDepth);
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

                ALenum format = GetOpenALBufferFormat(info.NumChannels, 32);
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

                ALenum format = GetOpenALBufferFormat(info.NumChannels, 16);
                alBufferData(bufferId, format, sampleBuffer, bufferSize, info.SampleRate);

                te_delete(sampleBuffer);
            }
            else
            {
                ALenum format = GetOpenALBufferFormat(info.NumChannels, info.BitDepth);
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

    void OAAudio::ClearContexts()
    {
        alcMakeContextCurrent(nullptr);

        for (auto& context : _contexts)
            alcDestroyContext(context);

        _contexts.clear();
    }

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

    void OAAudio::StartStreaming(OAAudioSource* source)
    {
        Lock lock(_mutex);

        _streamingCommandQueue.push_back({ StreamingCommandType::Start, source });
        _destroyedSources.erase(source);
    }

    void OAAudio::StopStreaming(OAAudioSource* source)
    {
        Lock lock(_mutex);

        _streamingCommandQueue.push_back({ StreamingCommandType::Stop, source });
        _destroyedSources.insert(source);
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
