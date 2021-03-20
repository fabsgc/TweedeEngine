set (TE_OPENAUDIO_INC_NOFILTER
    "TeOAPrerequisites.h"
    "TeOAImporter.h"
    "TeOAAudio.h"
    "TeOAAudioClip.h"
    "TeOAAudioSource.h"
    "TeOAAudioListener.h"
    "TeAudioDecoder.h"
    "TeOggVorbisDecoder.h"
    "TeOggVorbisEncoder.h"
    "TeFLACDecoder.h"
    "TeWaveDecoder.h"
)

set (TE_OPENAUDIO_SRC_NOFILTER
    "TeOAImporter.cpp"
    "TeOAPlugin.cpp"
    "TeOAAudio.cpp"
    "TeOAAudioSource.cpp"
    "TeOAAudioListener.cpp"
    "TeOAAudioClip.cpp"
    "TeOggVorbisDecoder.cpp"
    "TeOggVorbisEncoder.cpp"
    "TeFLACDecoder.cpp"
    "TeWaveDecoder.cpp"
)

source_group ("" FILES ${TE_OPENAUDIO_SRC_NOFILTER} ${TE_OPENAUDIO_INC_NOFILTER})

set (TE_OPENAUDIO_SRC
    ${TE_OPENAUDIO_INC_NOFILTER}
    ${TE_OPENAUDIO_SRC_NOFILTER}
)
