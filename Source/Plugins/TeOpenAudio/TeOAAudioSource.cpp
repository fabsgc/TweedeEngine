#include "TeOAAudioSource.h"
#include "TeOAAudio.h"
#include "TeOAAudioClip.h"
#include "AL/al.h"

namespace te
{
    OAAudioSource::OAAudioSource()
        : _streamBuffers()
        , _busyBuffers()
    { 
        gOAAudio()._registerSource(this);
        Rebuild();
    }

    OAAudioSource::~OAAudioSource()
    { 
        Clear();
        gOAAudio()._unregisterSource(this);
    }

    void OAAudioSource::SetTransform(const Transform& transform)
    {
        AudioSource::SetTransform(transform);

        auto& contexts = gOAAudio()._getContexts();
        UINT32 numContexts = (UINT32)contexts.size();
        for (UINT32 i = 0; i < numContexts; i++)
        {
            if (contexts.size() > 1)
                alcMakeContextCurrent(contexts[i]);

            if (Is3D())
            {
                Vector3 position = transform.GetPosition();
                alSource3f(_sourceIDs[i], AL_POSITION, position.x, position.y, position.z);
            }
            else
                alSource3f(_sourceIDs[i], AL_POSITION, 0.0f, 0.0f, 0.0f);
        }
    }

    void OAAudioSource::SetClip(const HAudioClip& clip)
    {
        Stop();

        Lock lock(_mutex);
        AudioSource::SetClip(clip);

        ApplyClip();
    }

    void OAAudioSource::SetVelocity(const Vector3& velocity)
    {
        AudioSource::SetVelocity(velocity);

        auto& contexts = gOAAudio()._getContexts();
        UINT32 numContexts = (UINT32)contexts.size();
        for (UINT32 i = 0; i < numContexts; i++)
        {
            if (contexts.size() > 1)
                alcMakeContextCurrent(contexts[i]);

            if (Is3D())
                alSource3f(_sourceIDs[i], AL_VELOCITY, velocity.x, velocity.y, velocity.z);
            else
                alSource3f(_sourceIDs[i], AL_VELOCITY, 0.0f, 0.0f, 0.0f);
        }
    }

    void OAAudioSource::SetVolume(float volume)
    {
        AudioSource::SetVolume(volume);

        auto& contexts = gOAAudio()._getContexts();
        UINT32 numContexts = (UINT32)contexts.size();
        for (UINT32 i = 0; i < numContexts; i++)
        {
            if (contexts.size() > 1)
                alcMakeContextCurrent(contexts[i]);

            alSourcef(_sourceIDs[i], AL_GAIN, _volume);
        }
    }

    void OAAudioSource::SetPitch(float pitch)
    {
        AudioSource::SetPitch(pitch);

        auto& contexts = gOAAudio()._getContexts();
        UINT32 numContexts = (UINT32)contexts.size();
        for (UINT32 i = 0; i < numContexts; i++)
        {
            if (contexts.size() > 1)
                alcMakeContextCurrent(contexts[i]);

            alSourcef(_sourceIDs[i], AL_PITCH, pitch);
        }
    }

    void OAAudioSource::SetIsLooping(bool loop)
    {
        AudioSource::SetIsLooping(loop);

        // When streaming we handle looping manually
        if (RequiresStreaming())
            loop = false;

        auto& contexts = gOAAudio()._getContexts();
        UINT32 numContexts = (UINT32)contexts.size();
        for (UINT32 i = 0; i < numContexts; i++)
        {
            if (contexts.size() > 1)
                alcMakeContextCurrent(contexts[i]);

            alSourcei(_sourceIDs[i], AL_LOOPING, loop);
        }
    }

    void OAAudioSource::SetPriority(INT32 priority)
    {
        AudioSource::SetPriority(priority);
    }

    void OAAudioSource::SetMinDistance(float distance)
    {
        AudioSource::SetMinDistance(distance);

        auto& contexts = gOAAudio()._getContexts();
        UINT32 numContexts = (UINT32)contexts.size();
        for (UINT32 i = 0; i < numContexts; i++)
        {
            if (contexts.size() > 1)
                alcMakeContextCurrent(contexts[i]);

            alSourcef(_sourceIDs[i], AL_REFERENCE_DISTANCE, distance);
        }
    }

    void OAAudioSource::SetAttenuation(float attenuation)
    {
        AudioSource::SetAttenuation(attenuation);

        auto& contexts = gOAAudio()._getContexts();
        UINT32 numContexts = (UINT32)contexts.size();
        for (UINT32 i = 0; i < numContexts; i++)
        {
            if (contexts.size() > 1)
                alcMakeContextCurrent(contexts[i]);

            alSourcef(_sourceIDs[i], AL_ROLLOFF_FACTOR, attenuation);
        }
    }

    void OAAudioSource::SetTime(float time)
    {
        if (!_audioClip.IsLoaded())
            return;

        AudioSourceState state = GetState();
        Stop();

        bool needsStreaming = RequiresStreaming();
        float clipTime;
        {
            Lock lock(_mutex);

            if (!needsStreaming)
                clipTime = time;
            else
            {               
                _streamProcessedPosition = (UINT32)(time * _audioClip->GetFrequency() * _audioClip->GetNumChannels());
                _streamQueuedPosition = _streamProcessedPosition;
                clipTime = 0.0f;
            }
        }

        auto& contexts = gOAAudio()._getContexts();
        UINT32 numContexts = (UINT32)contexts.size();
        for (UINT32 i = 0; i < numContexts; i++)
        {
            if (contexts.size() > 1)
                alcMakeContextCurrent(contexts[i]);

            alSourcef(_sourceIDs[i], AL_SEC_OFFSET, clipTime);
        }

        if (state != AudioSourceState::Stopped)
            Play();

        if (state == AudioSourceState::Paused)
            Pause();
    }

    float OAAudioSource::GetTime() const
    {
        Lock lock(_mutex);

        auto& contexts = gOAAudio()._getContexts();

        if (contexts.size() > 1)
            alcMakeContextCurrent(contexts[0]);

        bool needsStreaming = RequiresStreaming();
        float time;
        if (!needsStreaming)
        {
            alGetSourcef(_sourceIDs[0], AL_SEC_OFFSET, &time);
            return time;
        }
        else
        {
            float timeOffset = 0.0f;
            if (_audioClip.IsLoaded())
                timeOffset = (float)_streamProcessedPosition / _audioClip->GetFrequency() / _audioClip->GetNumChannels();

            // When streaming, the returned offset is relative to the last queued buffer
            alGetSourcef(_sourceIDs[0], AL_SEC_OFFSET, &time);
            return timeOffset + time;
        }
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

    void OAAudioSource::Clear()
    {
        _savedState = GetState();
        _savedTime = GetTime();
        Stop();

        auto& contexts = gOAAudio()._getContexts();
        UINT32 numContexts = (UINT32)contexts.size();

        Lock lock(_mutex);
        for (UINT32 i = 0; i < numContexts; i++)
        {
            if (contexts.size() > 1)
                alcMakeContextCurrent(contexts[i]);

            alSourcei(_sourceIDs[i], AL_BUFFER, 0);
            alDeleteSources(1, &_sourceIDs[i]);
        }

        _sourceIDs.clear();
    }

    void OAAudioSource::Rebuild()
    {
        auto& contexts = gOAAudio()._getContexts();
        UINT32 numContexts = (UINT32)contexts.size();

        {
            Lock lock(_mutex);

            for (UINT32 i = 0; i < numContexts; i++)
            {
                if (contexts.size() > 1)
                    alcMakeContextCurrent(contexts[i]);

                UINT32 source = 0;
                alGenSources(1, &source);

                _sourceIDs.push_back(source);
            }
        }

        for (UINT32 i = 0; i < numContexts; i++)
        {
            if (contexts.size() > 1)
                alcMakeContextCurrent(contexts[i]);

            alSourcef(_sourceIDs[i], AL_PITCH, _pitch);
            alSourcef(_sourceIDs[i], AL_REFERENCE_DISTANCE, _minDistance);
            alSourcef(_sourceIDs[i], AL_ROLLOFF_FACTOR, _attenuation);

            if (RequiresStreaming())
                alSourcei(_sourceIDs[i], AL_LOOPING, false);
            else
                alSourcei(_sourceIDs[i], AL_LOOPING, _loop);

            if (Is3D())
            {
                Vector3 position = _transform.GetPosition();

                alSourcei(_sourceIDs[i], AL_SOURCE_RELATIVE, false);
                alSource3f(_sourceIDs[i], AL_POSITION, position.x, position.y, position.z);
                alSource3f(_sourceIDs[i], AL_VELOCITY, _velocity.x, _velocity.y, _velocity.z);
            }
            else
            {
                alSourcei(_sourceIDs[i], AL_SOURCE_RELATIVE, true);
                alSource3f(_sourceIDs[i], AL_POSITION, 0.0f, 0.0f, 0.0f);
                alSource3f(_sourceIDs[i], AL_VELOCITY, 0.0f, 0.0f, 0.0f);
            }

            {
                Lock lock(_mutex);

                if (!_isStreaming)
                {
                    UINT32 oaBuffer = 0;
                    if (_audioClip.IsLoaded())
                    {
                        OAAudioClip* oaClip = static_cast<OAAudioClip*>(_audioClip.Get());
                        oaBuffer = oaClip->_getOpenALBuffer();
                    }

                    alSourcei(_sourceIDs[i], AL_BUFFER, oaBuffer);
                }
            }
        }

        SetTime(_savedTime);

        if (_savedState != AudioSourceState::Stopped)
            Play();

        if (_savedState == AudioSourceState::Paused)
            Pause();
    }

    void OAAudioSource::Stream()
    {
        // TODO
    }

    void OAAudioSource::StreamUnlocked()
    {
        // TODO
    }

    void OAAudioSource::StartStreaming()
    {
        // TODO
    }

    void OAAudioSource::StopStreaming()
    {
        // TODO
    }

    void OAAudioSource::SetGlobalPause(bool pause)
    {
        if (_globallyPaused == pause)
            return;

        _globallyPaused = pause;

        if (GetState() == AudioSourceState::Playing)
        {
            if (pause)
            {
                auto& contexts = gOAAudio()._getContexts();
                UINT32 numContexts = (UINT32)contexts.size();
                for (UINT32 i = 0; i < numContexts; i++)
                {
                    if (contexts.size() > 1)
                        alcMakeContextCurrent(contexts[i]);

                    alSourcePause(_sourceIDs[i]);
                }
            }
            else
            {
                Play();
            }
        }
    }

    bool OAAudioSource::Is3D() const
    {
        if (!_audioClip.IsLoaded())
            return true;

        return _audioClip->Is3D();
    }

    bool OAAudioSource::RequiresStreaming() const
    {
        if (!_audioClip.IsLoaded())
            return false;

        AudioReadMode readMode = _audioClip->GetReadMode();
        bool isCompressed = readMode == AudioReadMode::LoadCompressed && _audioClip->GetFormat() != AudioFormat::PCM;

        return (readMode == AudioReadMode::Stream) || isCompressed;
    }

    bool OAAudioSource::FillBuffer(UINT32 buffer, AudioDataInfo& info, UINT32 maxNumSamples)
    {
        // TODO
        return false;
    }

    void OAAudioSource::ApplyClip()
    {
        auto& contexts = gOAAudio()._getContexts();
        UINT32 numContexts = (UINT32)contexts.size();
        for (UINT32 i = 0; i < numContexts; i++)
        {
            if (contexts.size() > 1)
                alcMakeContextCurrent(contexts[i]);

            alSourcei(_sourceIDs[i], AL_SOURCE_RELATIVE, !Is3D());

            if (!RequiresStreaming())
            {
                UINT32 oaBuffer = 0;
                if (_audioClip.IsLoaded())
                {
                    OAAudioClip* oaClip = static_cast<OAAudioClip*>(_audioClip.Get());
                    oaBuffer = oaClip->_getOpenALBuffer();
                }

                alSourcei(_sourceIDs[i], AL_BUFFER, oaBuffer);
            }
        }

        // Looping is influenced by streaming mode, so re-apply it in case it changed
        SetIsLooping(_loop);
    }
}
