#pragma once

#include "TeOAPrerequisites.h"
#include "TeAudioDecoder.h"

namespace te
{
     /** Decodes .WAV audio data into raw PCM format. */
    class WaveDecoder : public AudioDecoder
    {
    public:
        WaveDecoder() = default;

        /** @copydoc AudioDecoder::Open */
        bool Open(const SPtr<DataStream>& stream, AudioDataInfo& info, UINT32 offset = 0) override;

        /** @copydoc AudioDecoder::Read */
        UINT32 Read(UINT8* samples, UINT32 numSamples) override;

        /** @copydoc AudioDecoder::Seek */
        void Seek(UINT32 offset) override;

        /** @copydoc AudioDecoder::isValid */
        bool IsValid(const SPtr<DataStream>& stream, UINT32 offset = 0) override;
    private:
        /** Parses the WAVE header and output audio file meta-data. Returns false if the header is not valid. */
        bool ParseHeader(AudioDataInfo& info);

        SPtr<DataStream> _stream;
        UINT32 _dataOffset = 0;
        UINT32 _bytesPerSample = 0;

        static const UINT32 MAIN_CHUNK_SIZE = 12;
    };
}
