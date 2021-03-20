#include "TeWaveDecoder.h"
#include "Utility/TeDataStream.h"

namespace te
{
#define WAVE_FORMAT_PCM			0x0001
#define WAVE_FORMAT_EXTENDED	0xFFFE

    bool WaveDecoder::IsValid(const SPtr<DataStream>& stream, UINT32 offset)
    {
        stream->Seek(offset);

        INT8 header[MAIN_CHUNK_SIZE];
        if (stream->Read(header, sizeof(header)) < (sizeof(header)))
            return false;

        return (header[0] == 'R') && (header[1] == 'I') && (header[2] == 'F') && (header[3] == 'F')
            && (header[8] == 'W') && (header[9] == 'A') && (header[10] == 'V') && (header[11] == 'E');
    }

    bool WaveDecoder::Open(const SPtr<DataStream>& stream, AudioDataInfo& info, UINT32 offset)
    {
        if (stream == nullptr)
            return false;

        _stream = stream;
        _stream->Seek(offset + MAIN_CHUNK_SIZE);

        if (!ParseHeader(info))
        {
            TE_DEBUG("Provided file is not a valid WAVE file.");
            return false;
        }

        return true;
    }

    void WaveDecoder::Seek(UINT32 offset)
    {
        _stream->Seek(_dataOffset + offset * _bytesPerSample);
    }

    UINT32 WaveDecoder::Read(UINT8* samples, UINT32 numSamples)
    {
        UINT32 numRead = (UINT32)_stream->Read(samples, numSamples * _bytesPerSample);

        if (_bytesPerSample == 1) // 8-bit samples are stored as unsigned, but engine convention is to store all bit depths as signed
        {
            for (UINT32 i = 0; i < numRead; i++)
            {
                INT8 val = samples[i] - 128;
                samples[i] = *((UINT8*)&val);
            }
        }

        return numRead;
    }

    bool WaveDecoder::ParseHeader(AudioDataInfo& info)
    {
        bool foundData = false;
        while (!foundData)
        {
            // Get sub-chunk ID and size
            UINT8 subChunkId[4];
            if (_stream->Read(subChunkId, sizeof(subChunkId)) != sizeof(subChunkId))
                return false;

            UINT32 subChunkSize = 0;
            if (_stream->Read(&subChunkSize, sizeof(subChunkSize)) != sizeof(subChunkSize))
                return false;

            // FMT chunk
            if (subChunkId[0] == 'f' && subChunkId[1] == 'm' && subChunkId[2] == 't' && subChunkId[3] == ' ')
            {
                UINT16 format = 0;
                if (_stream->Read(&format, sizeof(format)) != sizeof(format))
                    return false;

                if (format != WAVE_FORMAT_PCM && format != WAVE_FORMAT_EXTENDED)
                {
                    TE_DEBUG("Wave file doesn't contain raw PCM data. Not supported.");
                    return false;
                }

                UINT16 numChannels = 0;
                if (_stream->Read(&numChannels, sizeof(numChannels)) != sizeof(numChannels))
                    return false;

                UINT32 sampleRate = 0;
                if (_stream->Read(&sampleRate, sizeof(sampleRate)) != sizeof(sampleRate))
                    return false;

                UINT32 byteRate = 0;
                if (_stream->Read(&byteRate, sizeof(byteRate)) != sizeof(byteRate))
                    return false;

                UINT16 blockAlign = 0;
                if (_stream->Read(&blockAlign, sizeof(blockAlign)) != sizeof(blockAlign))
                    return false;

                UINT16 bitDepth = 0;
                if (_stream->Read(&bitDepth, sizeof(bitDepth)) != sizeof(bitDepth))
                    return false;

                info.NumChannels = numChannels;
                info.SampleRate = sampleRate;
                info.BitDepth = bitDepth;

                if (bitDepth != 8 && bitDepth != 16 && bitDepth != 24 && bitDepth != 32)
                {
                    TE_DEBUG("Unsupported number of bits per sample: {0}", bitDepth);
                    return false;
                }

                // Read extension data, and get the actual format
                if (format == WAVE_FORMAT_EXTENDED)
                {
                    UINT16 extensionSize = 0;
                    if (_stream->Read(&extensionSize, sizeof(extensionSize)) != sizeof(extensionSize))
                        return false;

                    if (extensionSize != 22)
                    {
                        TE_DEBUG("Wave file doesn't contain raw PCM data. Not supported.");
                        return false;
                    }

                    UINT16 validBitDepth = 0;
                    if (_stream->Read(&validBitDepth, sizeof(validBitDepth)) != sizeof(validBitDepth))
                        return false;

                    UINT32 channelMask = 0;
                    if (_stream->Read(&channelMask, sizeof(channelMask)) != sizeof(channelMask))
                        return false;

                    UINT8 subFormat[16];
                    if (_stream->Read(subFormat, sizeof(subFormat)) != sizeof(subFormat))
                        return false;

                    memcpy(&format, subFormat, sizeof(format));
                    if (format != WAVE_FORMAT_PCM)
                    {
                        TE_DEBUG("Wave file doesn't contain raw PCM data. Not supported.");
                        return false;
                    }
                }

                _bytesPerSample = bitDepth / 8;
            }
            // DATA chunk
            else if (subChunkId[0] == 'd' && subChunkId[1] == 'a' && subChunkId[2] == 't' && subChunkId[3] == 'a')
            {
                info.NumSamples = subChunkSize / _bytesPerSample;
                _dataOffset = (UINT32)_stream->Tell();

                foundData = true;
            }
            // Unsupported chunk type
            else
            {
                _stream->Skip(subChunkSize);
                if (_stream->Eof())
                    return false;
            }
        }

        return true;
    }
}
