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
        gOAAudio().RegisterSource(this);
        Rebuild();
    }

    OAAudioSource::~OAAudioSource()
    { 
        Clear();
        gOAAudio().UnregisterSource(this);
    }

    void OAAudioSource::SetTransform(const Transform& transform)
    {
        AudioSource::SetTransform(transform);

        auto& contexts = gOAAudio().GetContexts();
        UINT32 numContexts = (UINT32)contexts.size();
        for (UINT32 i = 0; i < numContexts; i++)
        {
            if (contexts.size() > 1)
                alcMakeContextCurrent(contexts[i]);

            if (Is3D() && GetIsPlay3D())
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

        auto& contexts = gOAAudio().GetContexts();
        UINT32 numContexts = (UINT32)contexts.size();
        for (UINT32 i = 0; i < numContexts; i++)
        {
            if (contexts.size() > 1)
                alcMakeContextCurrent(contexts[i]);

            if (Is3D() && GetIsPlay3D())
                alSource3f(_sourceIDs[i], AL_VELOCITY, velocity.x, velocity.y, velocity.z);
            else
                alSource3f(_sourceIDs[i], AL_VELOCITY, 0.0f, 0.0f, 0.0f);
        }
    }

    void OAAudioSource::SetVolume(float volume)
    {
        AudioSource::SetVolume(volume);

        auto& contexts = gOAAudio().GetContexts();
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

        auto& contexts = gOAAudio().GetContexts();
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

        auto& contexts = gOAAudio().GetContexts();
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

        auto& contexts = gOAAudio().GetContexts();
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

        auto& contexts = gOAAudio().GetContexts();
        UINT32 numContexts = (UINT32)contexts.size();
        for (UINT32 i = 0; i < numContexts; i++)
        {
            if (contexts.size() > 1)
                alcMakeContextCurrent(contexts[i]);

            alSourcef(_sourceIDs[i], AL_ROLLOFF_FACTOR, attenuation);
        }
    }

    void OAAudioSource::SetIsPlay3D(bool play3D)
    {
        AudioSource::SetIsPlay3D(play3D);
        ApplyClip();
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

        auto& contexts = gOAAudio().GetContexts();
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

        auto& contexts = gOAAudio().GetContexts();

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
        if (_globallyPaused)
            return;

        if(GetState() == AudioSourceState::Playing)
            return;

        if (RequiresStreaming())
        {
            Lock lock(_mutex);

            if (!_isStreaming)
            {
                StartStreaming();
                StreamUnlocked(); // Stream first block on this thread to ensure something can play right away
            }
        }

        auto& contexts = gOAAudio().GetContexts();
        UINT32 numContexts = (UINT32)contexts.size();
        for (UINT32 i = 0; i < numContexts; i++)
        {
            if (contexts.size() > 1)
                alcMakeContextCurrent(contexts[i]);

            alSourcePlay(_sourceIDs[i]);

            // Non-3D clips need to play only on a single source
            // Note: I'm still creating sourcs objects (and possibly queuing streaming buffers) for these non-playing
            // sources. It would be possible to optimize them out at cost of more complexity. At this time it doesn't feel
            // worth it.
            if (!Is3D())
                break;
        }
    }

    void OAAudioSource::Pause()
    {
        auto& contexts = gOAAudio().GetContexts();
        UINT32 numContexts = (UINT32)contexts.size();
        for (UINT32 i = 0; i < numContexts; i++)
        {
            if (contexts.size() > 1)
                alcMakeContextCurrent(contexts[i]);

            alSourcePause(_sourceIDs[i]);
        }
    }

    void OAAudioSource::Stop()
    { 
        auto& contexts = gOAAudio().GetContexts();
        UINT32 numContexts = (UINT32)contexts.size();
        for (UINT32 i = 0; i < numContexts; i++)
        {
            if (contexts.size() > 1)
                alcMakeContextCurrent(contexts[i]);

            alSourceStop(_sourceIDs[i]);
            alSourcef(_sourceIDs[i], AL_SEC_OFFSET, 0.0f);
        }

        {
            Lock lock(_mutex);

            _streamProcessedPosition = 0;
            _streamQueuedPosition = 0;

            if (_isStreaming)
                StopStreaming();
        }
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

        auto& contexts = gOAAudio().GetContexts();
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
        auto& contexts = gOAAudio().GetContexts();
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

            if (Is3D() && GetIsPlay3D())
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
                        oaBuffer = oaClip->GetOpenALBuffer();
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
        Lock lock(_mutex);

        StreamUnlocked();
    }

    void OAAudioSource::StreamUnlocked()
    {
        AudioDataInfo info;
        info.BitDepth = _audioClip->GetBitDepth();
        info.NumChannels = _audioClip->GetNumChannels();
        info.SampleRate = _audioClip->GetFrequency();
        info.NumSamples = 0;

        UINT32 totalNumSamples = _audioClip->GetNumSamples();

        // Note: It is safe to access contexts here only because it is guaranteed by the OAAudio manager that it will always
        // stop all streaming before changing contexts. Otherwise a mutex lock would be needed for every context access.
        auto& contexts = gOAAudio().GetContexts();
        UINT32 numContexts = (UINT32)contexts.size();
        for (UINT32 i = 0; i < numContexts; i++)
        {
            if (contexts.size() > 1)
                alcMakeContextCurrent(contexts[i]);

            INT32 numProcessedBuffers = 0;
            alGetSourcei(_sourceIDs[i], AL_BUFFERS_PROCESSED, &numProcessedBuffers);

            for (INT32 j = numProcessedBuffers; j > 0; j--)
            {
                UINT32 buffer;
                alSourceUnqueueBuffers(_sourceIDs[i], 1, &buffer);

                INT32 bufferIdx = -1;
                for (UINT32 k = 0; k < _streamBufferCount; k++)
                {
                    if (buffer == _streamBuffers[k])
                    {
                        bufferIdx = k;
                        break;
                    }
                }

                // Possibly some buffer from previous playback remained unqueued, in which case ignore it
                if (bufferIdx == -1)
                    continue;

                _busyBuffers[bufferIdx] &= ~(1 << bufferIdx);

                // Check if all sources are done with this buffer
                if (_busyBuffers[bufferIdx] != 0)
                    break;

                INT32 bufferSize;
                INT32 bufferBits;

                alGetBufferi(buffer, AL_SIZE, &bufferSize);
                alGetBufferi(buffer, AL_BITS, &bufferBits);

                if (bufferBits == 0)
                {
                    TE_DEBUG("Error decoding stream.");
                    return;
                }
                else
                {
                    UINT32 bytesPerSample = bufferBits / 8;
                    _streamProcessedPosition += bufferSize / bytesPerSample;
                }

                if (_streamProcessedPosition == totalNumSamples) // Reached the end
                {
                    _streamProcessedPosition = 0;

                    if (!_loop) // Variable used on both threads and not thread safe, but it doesn't matter
                    {
                        StopStreaming();
                        return;
                    }
                }
            }
        }

        for (UINT32 i = 0; i < _streamBufferCount; i++)
        {
            if (_busyBuffers[i] != 0)
                continue;

            if (FillBuffer(_streamBuffers[i], info, totalNumSamples))
            {
                for (auto& source : _sourceIDs)
                    alSourceQueueBuffers(source, 1, &_streamBuffers[i]);

                _busyBuffers[i] |= 1 << i;
            }
            else
                break;
        }
    }

    void OAAudioSource::StartStreaming()
    {
        assert(!_isStreaming);

        alGenBuffers(_streamBufferCount, _streamBuffers);
        gOAAudio().StartStreaming(this);

        memset(&_busyBuffers, 0, sizeof(_busyBuffers));
        _isStreaming = true;
    }

    void OAAudioSource::StopStreaming()
    {
        assert(_isStreaming);

        _isStreaming = false;
        gOAAudio().StopStreaming(this);

        auto& contexts = gOAAudio().GetContexts();
        UINT32 numContexts = (UINT32)contexts.size();
        for (UINT32 i = 0; i < numContexts; i++)
        {
            if (contexts.size() > 1)
                alcMakeContextCurrent(contexts[i]);

            INT32 numQueuedBuffers;
            alGetSourcei(_sourceIDs[i], AL_BUFFERS_QUEUED, &numQueuedBuffers);

            UINT32 buffer;
            for (INT32 j = 0; j < numQueuedBuffers; j++)
                alSourceUnqueueBuffers(_sourceIDs[i], 1, &buffer);
        }

        alDeleteBuffers(_streamBufferCount, _streamBuffers);
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
                auto& contexts = gOAAudio().GetContexts();
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
        UINT32 numRemainingSamples = maxNumSamples - _streamQueuedPosition;
        if (numRemainingSamples == 0) // Reached the end
        {
            if (_loop)
            {
                _streamQueuedPosition = 0;
                numRemainingSamples = maxNumSamples;
            }
            else // If not looping, don't queue any more buffers, we're done
                return false;
        }

        // Read audio data
        UINT32 numSamples = std::min(numRemainingSamples, info.SampleRate * info.NumChannels); // 1 second of data
        UINT32 sampleBufferSize = numSamples * (info.BitDepth / 8);

        UINT8* samples = (UINT8*)te_allocate(sampleBufferSize);

        OAAudioClip* audioClip = static_cast<OAAudioClip*>(_audioClip.Get());

        audioClip->GetSamples(samples, _streamQueuedPosition, numSamples);
        _streamQueuedPosition += numSamples;

        info.NumSamples = numSamples;
        gOAAudio().WriteToOpenALBuffer(buffer, samples, info);

        te_delete(samples);

        return true;
    }

    void OAAudioSource::ApplyClip()
    {
        auto& contexts = gOAAudio().GetContexts();
        UINT32 numContexts = (UINT32)contexts.size();
        for (UINT32 i = 0; i < numContexts; i++)
        {
            if (contexts.size() > 1)
                alcMakeContextCurrent(contexts[i]);

            alSourcei(_sourceIDs[i], AL_SOURCE_RELATIVE, !(Is3D() && GetIsPlay3D()));

            if (!RequiresStreaming())
            {
                UINT32 oaBuffer = 0;
                if (_audioClip.IsLoaded())
                {
                    OAAudioClip* oaClip = static_cast<OAAudioClip*>(_audioClip.Get());
                    oaBuffer = oaClip->GetOpenALBuffer();
                }

                alSourcei(_sourceIDs[i], AL_BUFFER, oaBuffer);
            }
        }

        // Looping is influenced by streaming mode, so re-apply it in case it changed
        SetIsLooping(_loop);
    }
}
