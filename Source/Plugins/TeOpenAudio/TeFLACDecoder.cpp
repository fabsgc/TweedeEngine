#include "TeFLACDecoder.h"
#include "Utility/TeDataStream.h"

namespace te
{
    FLAC__StreamDecoderReadStatus StreamRead(const FLAC__StreamDecoder*, FLAC__byte buffer[], size_t* bytes, void* clientData)
    {
        FLACDecoderData* data = static_cast<FLACDecoderData*>(clientData);

        INT64 count = (INT64)data->Stream->Read(buffer, *bytes);
        if (count > 0)
        {
            *bytes = (size_t)count;
            return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
        }

        if (count == 0)
            return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;

        return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
    }

    FLAC__StreamDecoderSeekStatus StreamSeek(const FLAC__StreamDecoder*, FLAC__uint64 absoluteByteOffset, void* clientData)
    {
        FLACDecoderData* data = static_cast<FLACDecoderData*>(clientData);

        data->Stream->Seek(data->StreamOffset + (UINT32)absoluteByteOffset);
        INT64 position = (INT64)(data->Stream->Tell() - data->StreamOffset);
        if (position >= 0)
            return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
        else
            return FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;
    }

    FLAC__StreamDecoderTellStatus StreamTell(const FLAC__StreamDecoder*, FLAC__uint64* absoluteByteOffset, void* clientData)
    {
        FLACDecoderData* data = (FLACDecoderData*)(clientData);

        INT64 position = (INT64)(data->Stream->Tell() - data->StreamOffset);
        if (position >= 0)
        {
            *absoluteByteOffset = position;
            return FLAC__STREAM_DECODER_TELL_STATUS_OK;
        }
        else
        {
            return FLAC__STREAM_DECODER_TELL_STATUS_ERROR;
        }
    }

    FLAC__StreamDecoderLengthStatus StreamLength(const FLAC__StreamDecoder*, FLAC__uint64* streamLength, void* clientData)
    {
        FLACDecoderData* data = (FLACDecoderData*)(clientData);

        *streamLength = data->Stream->Size() - data->StreamOffset;
        return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
    }

    FLAC__bool StreamEof(const FLAC__StreamDecoder*, void* clientData)
    {
        FLACDecoderData* data = (FLACDecoderData*)(clientData);

        return data->Stream->Eof();
    }

    FLAC__StreamDecoderWriteStatus StreamWrite(const FLAC__StreamDecoder*, const FLAC__Frame* frame, const FLAC__int32* const buffer[], void* clientData)
    {
        FLACDecoderData* data = (FLACDecoderData*)(clientData);

        if (!data->Output) // Seek
            return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;

        UINT32 bytesPerSample = data->Info.BitDepth / 8;

        // If we received more data than we need, store it in the overflow buffer
        UINT32 frameSamples = frame->header.blocksize * frame->header.channels;
        if (data->SamplesToRead < frameSamples)
        {
            UINT32 numExtraSamples = frameSamples - data->SamplesToRead;
            UINT32 extraBytes = numExtraSamples * bytesPerSample;
            data->Overflow.reserve(extraBytes);
        }

        assert(bytesPerSample <= 4);
        for (UINT32 i = 0; i < frame->header.blocksize; i++)
        {
            for (UINT32 j = 0; j < frame->header.channels; j++)
            {
                if (data->SamplesToRead > 0)
                {
                    memcpy(data->Output, &buffer[j][i], bytesPerSample);

                    data->Output += bytesPerSample;
                    data->SamplesToRead--;
                }
                else
                {
                    UINT8 sample[4];
                    memcpy(sample, &buffer[j][i], bytesPerSample);

                    for (UINT32 k = 0; k < bytesPerSample; k++)
                        data->Overflow.push_back(sample[k]);
                }
            }
        }

        return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
    }

    void StreamMetadata(const FLAC__StreamDecoder*, const FLAC__StreamMetadata* meta, void* clientData)
    {
        FLACDecoderData* data = (FLACDecoderData*)(clientData);

        if (meta->type == FLAC__METADATA_TYPE_STREAMINFO)
        {
            data->Info.NumSamples = (UINT32)meta->data.stream_info.total_samples * meta->data.stream_info.channels;
            data->Info.SampleRate = meta->data.stream_info.sample_rate;
            data->Info.NumChannels = meta->data.stream_info.channels;
            data->Info.BitDepth = meta->data.stream_info.bits_per_sample;
        }
    }

    void StreamError(const FLAC__StreamDecoder*, FLAC__StreamDecoderErrorStatus, void* clientData)
    {
        FLACDecoderData* data = (FLACDecoderData*)(clientData);
        data->Error = true;
    }

    FLACDecoder::~FLACDecoder()
    {
        Close();
    }

    bool FLACDecoder::IsValid(const SPtr<DataStream>& stream, UINT32 offset)
    {
        stream->Seek(offset);

        FLAC__StreamDecoder* decoder = FLAC__stream_decoder_new();
        if (!decoder)
            return false;

        FLACDecoderData data;
        data.Stream = stream;
        _data.StreamOffset = offset;
        FLAC__stream_decoder_init_stream(decoder, &StreamRead, &StreamSeek, &StreamTell, &StreamLength, &StreamEof,
            &StreamWrite, nullptr, &StreamError, &data);

        bool valid = FLAC__stream_decoder_process_until_end_of_metadata(decoder) != 0;

        FLAC__stream_decoder_finish(decoder);
        FLAC__stream_decoder_delete(decoder);

        return valid && !data.Error;
    }

    bool FLACDecoder::Open(const SPtr<DataStream>& stream, AudioDataInfo& info, UINT32 offset)
    {
        if (stream == nullptr)
            return false;

        stream->Seek(offset);

        _decoder = FLAC__stream_decoder_new();
        if (_decoder == nullptr)
        {
            TE_DEBUG("Failed to open a FLAC file.");
            return false;
        }

        _data.Stream = stream;
        _data.StreamOffset = offset;
        FLAC__stream_decoder_init_stream(_decoder, &StreamRead, &StreamSeek, &StreamTell, &StreamLength, &StreamEof,
            &StreamWrite, &StreamMetadata, &StreamError, &_data);

        if (!FLAC__stream_decoder_process_until_end_of_metadata(_decoder))
        {
            Close();
            TE_DEBUG("Failed to open a FLAC file.");
            return false;
        }

        info = _data.Info;

        return true;
    }

    void FLACDecoder::Seek(UINT32 offset)
    {
        _data.Output = nullptr;
        _data.SamplesToRead = 0;
        _data.Overflow.clear();

        FLAC__stream_decoder_seek_absolute(_decoder, offset);
    }

    UINT32 FLACDecoder::Read(UINT8* samples, UINT32 numSamples)
    {
        UINT32 overflowSize = (UINT32)_data.Overflow.size();
        UINT32 overflowNumSamples = 0;

        UINT32 bytesPerSample = _data.Info.BitDepth / 8;
        if (overflowSize > 0)
        {
            UINT32 sampleSize = numSamples * bytesPerSample;
            if (overflowSize > sampleSize)
            {
                std::copy(_data.Overflow.begin(), _data.Overflow.begin() + sampleSize, samples);
                _data.Overflow.erase(_data.Overflow.begin(), _data.Overflow.begin() + sampleSize);

                return numSamples;
            }
            else
                std::copy(_data.Overflow.begin(), _data.Overflow.end(), samples);

            overflowNumSamples = overflowSize / bytesPerSample;
        }

        _data.Output = samples + overflowSize;
        _data.SamplesToRead = numSamples - overflowNumSamples;
        _data.Overflow.clear();

        while (_data.SamplesToRead > 0)
        {
            if (!FLAC__stream_decoder_process_single(_decoder))
                break;

            if (FLAC__stream_decoder_get_state(_decoder) == FLAC__STREAM_DECODER_END_OF_STREAM)
                break;
        }

        return numSamples - _data.SamplesToRead;
    }

    void FLACDecoder::Close()
    {
        if (_decoder != nullptr)
        {
            FLAC__stream_decoder_finish(_decoder);
            FLAC__stream_decoder_delete(_decoder);
            _decoder = nullptr;
        }
    }
}
