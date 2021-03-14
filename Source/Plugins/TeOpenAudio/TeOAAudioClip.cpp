#include "TeOAAudioClip.h"
// #include "TeOggVorbisEncoder.h"
#include "TeOggVorbisDecoder.h"
#include "Utility/TeDataStream.h"
#include "TeOAAudio.h"
#include "AL/al.h"

namespace te
{
    OAAudioClip::OAAudioClip(const SPtr<DataStream>& samples, UINT32 streamSize, UINT32 numSamples, const AUDIO_CLIP_DESC& desc)
        : AudioClip(samples, streamSize, numSamples, desc)
    { }

    OAAudioClip::~OAAudioClip()
    {
        if (_bufferId != (UINT32)-1)
            alDeleteBuffers(1, &_bufferId);
    }

    void OAAudioClip::Initialize()
    {
        {
            Lock lock(_mutex); // Needs to be called even if stream data is null, to ensure memory fence is added so the
                               // other thread sees properly initialized AudioClip members

            AudioDataInfo info;
            info.BitDepth = _desc.BitDepth;
            info.NumChannels = _desc.NumChannels;
            info.NumSamples = _numSamples;
            info.SampleRate = _desc.Frequency;

            // Load decompressed data into a sound buffer
            bool loadDecompressed =
                _desc.ReadMode == AudioReadMode::LoadDecompressed ||
                (_desc.ReadMode == AudioReadMode::LoadCompressed && _desc.Format == AudioFormat::PCM);

            if (loadDecompressed)
            {
                // Read all data into memory
                SPtr<DataStream> stream;
                UINT32 offset = 0;
                if (_sourceStreamData != nullptr) // If it's already loaded in memory, use it directly
                    stream = _sourceStreamData;
                else
                {
                    stream = _streamData;
                    offset = _streamOffset;
                }

                UINT32 bufferSize = info.NumSamples * (info.BitDepth / 8);
                UINT8* sampleBuffer = (UINT8*)te_allocate(bufferSize);

                // Decompress from Ogg
                if (_desc.Format == AudioFormat::VORBIS)
                {
                    OggVorbisDecoder reader;
                    if (reader.Open(stream, info, offset))
                        reader.Read(sampleBuffer, info.NumSamples);
                    else
                        TE_DEBUG("Failed decompressing AudioClip stream.");
                }
                // Load directly
                else
                {
                    stream->Seek(offset);
                    stream->Read(sampleBuffer, bufferSize);
                }

                alGenBuffers(1, &_bufferId);
                gOAAudio()._writeToOpenALBuffer(_bufferId, sampleBuffer, info);

                _streamData = nullptr;
                _streamOffset = 0;
                _streamSize = 0;

                te_delete(sampleBuffer);
            }
            // Load compressed data for streaming from memory
            else if (_desc.ReadMode == AudioReadMode::LoadCompressed)
            {
                // If reading from file, make a copy of data in memory, otherwise just take ownership of the existing buffer
                if (_streamData->IsFile())
                {
                    if (_sourceStreamData != nullptr) // If it's already loaded in memory, use it directly
                        _streamData = _sourceStreamData;
                    else
                    {
                        auto memStream = te_shared_ptr_new<MemoryDataStream>(_streamSize);

                        _streamData->Seek(_streamOffset);
                        _streamData->Read(memStream->Data(), _streamSize);

                        _streamData = memStream;
                    }

                    _streamOffset = 0;
                }
            }
            // Keep original stream for streaming from file
            else
            {
                // Do nothing
            }

            if (_desc.Format == AudioFormat::VORBIS && _desc.ReadMode != AudioReadMode::LoadDecompressed)
            {
                _needsDecompression = true;

                if (_streamData != nullptr)
                {
                    if (!_vorbisReader.Open(_streamData, info, _streamOffset))
                        TE_DEBUG("Failed decompressing AudioClip stream.");
                }
            }
        }
    }

    void OAAudioClip::GetSamples(UINT8* samples, UINT32 offset, UINT32 count) const
    {
        Lock lock(_mutex);

        // Try to read from normal stream, and if that fails read from in-memory stream if it exists
        if (_streamData != nullptr)
        {
            if (_needsDecompression)
            {
                _vorbisReader.Seek(offset);
                _vorbisReader.Read(samples, count);
            }
            else
            {
                UINT32 bytesPerSample = _desc.BitDepth / 8;
                UINT32 size = count * bytesPerSample;
                UINT32 streamOffset = _streamOffset + offset * bytesPerSample;

                _streamData->Seek(streamOffset);
                _streamData->Read(samples, size);
            }

            return;
        }

        if (_sourceStreamData != nullptr)
        {
            assert(!_needsDecompression); // Normal stream must exist if decompressing

            const UINT32 bytesPerSample = _desc.BitDepth / 8;
            UINT32 size = count * bytesPerSample;
            UINT32 streamOffset = offset * bytesPerSample;

            _sourceStreamData->Seek(streamOffset);
            _sourceStreamData->Read(samples, size);
            return;
        }

        TE_DEBUG("Attempting to read samples while sample data is not available.");
    }

    SPtr<DataStream> OAAudioClip::GetSourceStream(UINT32& size)
    {
        Lock lock(_mutex);

        size = _sourceStreamSize;
        _sourceStreamData->Seek(0);

        return _sourceStreamData;
    }
}
