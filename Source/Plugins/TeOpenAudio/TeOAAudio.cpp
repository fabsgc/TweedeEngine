#include "TeOAAudio.h"
#include "TeOAAudioClip.h"
#include "Math/TeMath.h"
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

        // TODO
        // RebuildContexts();
    }

    OAAudio::~OAAudio()
    {
        StopManualSources();

        //assert(_listeners.empty() && _sources.empty()); // Everything should be destroyed at this point
        ClearContexts();

        if (_device != nullptr)
            alcCloseDevice(_device);
    }

    void OAAudio::SetVolume(float volume)
    {
        _volume = Math::Clamp01(volume);

        /*for (auto& listener : _listeners)
            listener->rebuild();*/
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

        /*for (auto& source : _sources)
            source->SetGlobalPause(paused);*/
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

    void OAAudio::RebuildContexts()
    {
        // TODO
    }

    /** Delete all existing OpenAL contexts. */
    void OAAudio::ClearContexts()
    {
        // TODO
    }

    /** Streams new data to audio sources that require it. */
    void OAAudio::UpdateStreaming()
    {
        // TODO
    }

    SPtr<AudioClip> OAAudio::CreateClip(const SPtr<DataStream>& samples, UINT32 streamSize, UINT32 numSamples,
        const AUDIO_CLIP_DESC& desc)
    {
        return te_core_ptr_new<OAAudioClip>(samples, streamSize, numSamples, desc);
    }
}
