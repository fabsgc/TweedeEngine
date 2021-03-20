#pragma once

#include "TeOAPrerequisites.h"
#include "TeAudioDecoder.h"
#include "FLAC/stream_decoder.h"

namespace te
{
     /** Data used by the FLAC decoder. */
    struct FLACDecoderData
    {
        SPtr<DataStream> Stream;
        UINT32 StreamOffset = 0;
        AudioDataInfo Info;
        UINT8* Output = nullptr;
        Vector<UINT8> Overflow;
        UINT32 SamplesToRead = 0;
        bool Error = false;
    };

    /** Decodes FLAC audio data into raw PCM samples. */
    class FLACDecoder : public AudioDecoder
    {
    public:
        FLACDecoder() = default;
        ~FLACDecoder();

        /** @copydoc AudioDecoder::Open */
        bool Open(const SPtr<DataStream>& stream, AudioDataInfo& info, UINT32 offset = 0) override;

        /** @copydoc AudioDecoder::Seek */
        void Seek(UINT32 offset) override;

        /** @copydoc AudioDecoder::Read */
        UINT32 Read(UINT8* samples, UINT32 numSamples) override;

        /** @copydoc AudioDecoder::IsValid */
        bool IsValid(const SPtr<DataStream>& stream, UINT32 offset = 0) override;
    private:
        /** Cleans up the FLAC decoder. */
        void Close();

        FLAC__StreamDecoder* _decoder = nullptr;
        FLACDecoderData _data;
    };
}
