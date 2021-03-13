set (TE_OPENAUDIO_INC_NOFILTER
    "TeOAPrerequisites.h"
    "TeOAImporter.h"
    "TeOAAudio.h"
    "TeOAAudioClip.h"
    "TeAudioDecoder.h"
    "TeOggVorbisDecoder.h"
)

set (TE_OPENAUDIO_SRC_NOFILTER
    "TeOAImporter.cpp"
    "TeOAPlugin.cpp"
    "TeOAAudio.cpp"
    "TeOAAudioClip.cpp"
    "TeOggVorbisDecoder.cpp"
)

source_group ("" FILES ${TE_OPENAUDIO_SRC_NOFILTER} ${TE_OPENAUDIO_INC_NOFILTER})

set (TE_OPENAUDIO_SRC
    ${TE_OPENAUDIO_INC_NOFILTER}
    ${TE_OPENAUDIO_SRC_NOFILTER}
)
