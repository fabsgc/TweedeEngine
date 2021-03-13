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
        }
    }

    SPtr<DataStream> OAAudioClip::GetSourceStream(UINT32& size)
    {
        Lock lock(_mutex);

        size = _sourceStreamSize;
        _sourceStreamData->Seek(0);

        return _sourceStreamData;
    }
}
