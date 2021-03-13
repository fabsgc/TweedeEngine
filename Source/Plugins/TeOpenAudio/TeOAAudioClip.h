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

    protected:
        /** @copydoc Resource::initialize */
        void Initialize() override;

        /** @copydoc AudioClip::getSourceStream */
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
