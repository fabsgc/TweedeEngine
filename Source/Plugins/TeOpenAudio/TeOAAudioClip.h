#pragma once

#include "TeOAPrerequisites.h"
#include "Audio/TeAudioClip.h"
#include "TeOggVorbisDecoder.h"

namespace te
{
    /** OpenAudio implementation of an AudioClip. */
    class OAAudioClip : public AudioClip
    {
    public:
        OAAudioClip(const SPtr<DataStream>& samples, UINT32 streamSize, UINT32 numSamples, const AUDIO_CLIP_DESC& desc);
        virtual ~OAAudioClip();

        /**
         * Returns audio samples in PCM format, channel data interleaved. Only available if the audio data has been created
         * with AudioReadMode::Stream, AudioReadMode::LoadCompressed (and the format is compressed), or if @p keepSourceData
         * was enabled on creation.
         *
         * @param[in]	samples		Previously allocated buffer to contain the samples.
         * @param[in]	offset		Offset in number of samples at which to start reading (should be a multiple of number
         *							of channels).
         * @param[in]	count		Number of samples to read (should be a multiple of number of channels).
         *
         * @note	Implementation must be thread safe as this will get called from audio streaming thread.
         */
        void GetSamples(UINT8* samples, UINT32 offset, UINT32 count) const;

        /** Returns the internal OpenAL buffer. Only valid if the audio clip was created without AudioReadMode::Stream. */
        UINT32 GetOpenALBuffer() const { return _bufferId; }

    protected:
        /** @copydoc Resource::Initialize */
        void Initialize() override;

        /** @copydoc AudioClip::GetSourceStream */
        SPtr<DataStream> GetSourceStream(UINT32& size) override;

    private:
        mutable Mutex _mutex;
        mutable OggVorbisDecoder _vorbisReader;
        bool _needsDecompression = false;
        UINT32 _bufferId = (UINT32)-1;

        // These streams exist to save original audio data in case it's needed later (usually for saving with the editor, or
        // manual data manipulation). In normal usage (in-game) these will be null so no memory is wasted.
        SPtr<DataStream> _sourceStreamData;
        UINT32 _sourceStreamSize = 0;
    };
}
