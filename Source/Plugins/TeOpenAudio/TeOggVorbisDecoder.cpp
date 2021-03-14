#include "TeOggVorbisDecoder.h"
#include "Utility/TeDataStream.h"
#include <vorbis/codec.h>

namespace te
{
    size_t oggRead(void* ptr, size_t size, size_t nmemb, void* data)
    {
        OggDecoderData* decoderData = static_cast<OggDecoderData*>(data);
        return static_cast<std::size_t>(decoderData->Stream->Read(ptr, size * nmemb));
    }

    int oggSeek(void* data, ogg_int64_t offset, int whence)
    {
        OggDecoderData* decoderData = static_cast<OggDecoderData*>(data);
        switch (whence)
        {
        case SEEK_SET:
            offset += decoderData->Offset;
            break;
        case SEEK_CUR:
            offset += decoderData->Stream->Tell();
            break;
        case SEEK_END:
            offset = std::max(0, (INT32)decoderData->Stream->Size() - 1);
            break;
        }

        decoderData->Stream->Seek((UINT32)offset);
        return (int)(decoderData->Stream->Tell() - decoderData->Offset);
    }

    long oggTell(void* data)
    {
        OggDecoderData* decoderData = static_cast<OggDecoderData*>(data);
        return (long)(decoderData->Stream->Tell() - decoderData->Offset);
    }

    static ov_callbacks callbacks = { &oggRead, &oggSeek, nullptr, &oggTell };

    OggVorbisDecoder::OggVorbisDecoder()
    {
        _oggVorbisFile.datasource = nullptr;
    }

    OggVorbisDecoder::~OggVorbisDecoder()
    {
        if (_oggVorbisFile.datasource != nullptr)
            ov_clear(&_oggVorbisFile);
    }

    bool OggVorbisDecoder::IsValid(const SPtr<DataStream>& stream, UINT32 offset)
	{
        stream->Seek(offset);
        _decoderData.Stream = stream;
        _decoderData.Offset = offset;

        OggVorbis_File file;
        if (ov_test_callbacks(&_decoderData, &file, nullptr, 0, callbacks) == 0)
        {
            ov_clear(&file);
            return true;
        }

        return false;
    }

    bool OggVorbisDecoder::Open(const SPtr<DataStream>& stream, AudioDataInfo& info, UINT32 offset)
	{
        if (stream == nullptr)
            return false;

        stream->Seek(offset);
        _decoderData.Stream = stream;
        _decoderData.Offset = offset;

        int status = ov_open_callbacks(&_decoderData, &_oggVorbisFile, nullptr, 0, callbacks);
        if (status < 0)
        {
            TE_DEBUG("Failed to open Ogg Vorbis file.");
            return false;
        }

        vorbis_info* vorbisInfo = ov_info(&_oggVorbisFile, -1);
        info.NumChannels = vorbisInfo->channels;
        info.SampleRate = vorbisInfo->rate;
        info.NumSamples = (UINT32)(ov_pcm_total(&_oggVorbisFile, -1) * vorbisInfo->channels);
        info.BitDepth = 16;

        _channelCount = info.NumChannels;
        return true;
    }

    void OggVorbisDecoder::Seek(UINT32 offset)
	{
        ov_pcm_seek(&_oggVorbisFile, offset / _channelCount);
    }

    UINT32 OggVorbisDecoder::Read(UINT8* samples, UINT32 numSamples)
	{
        UINT32 numReadSamples = 0;
        while (numReadSamples < numSamples)
        {
            INT32 bytesToRead = (INT32)(numSamples - numReadSamples) * sizeof(INT16);
            UINT32 bytesRead = ov_read(&_oggVorbisFile, (char*)samples, bytesToRead, 0, 2, 1, nullptr);
            if (bytesRead > 0)
            {
                UINT32 samplesRead = bytesRead / sizeof(INT16);
                numReadSamples += samplesRead;
                samples += samplesRead * sizeof(INT16);
            }
            else
                break;
        }

        return numReadSamples;
    }
}
