#pragma once

#include "TeCorePrerequisites.h"
#include "Importer/TeImportOptions.h"
#include "Audio/TeAudioClip.h"

namespace te
{
    /** Contains import options you may use to control how an audio clip is imported. */
    class TE_CORE_EXPORT AudioClipImportOptions : public ImportOptions
    {
    public:
        AudioClipImportOptions();

        /** Audio format to import the audio clip as. */
        AudioFormat Format = AudioFormat::PCM;

        /** Determines how is audio data loaded into memory. */
        AudioReadMode ReadMode = AudioReadMode::LoadDecompressed;

        /**
         * Determines should the clip be played as spatial (3D) audio or as normal audio. 3D clips will be converted
         * to mono on import.
         */
        bool Is3D = true;

        /** Size of a single sample in bits. The clip will be converted to this bit depth on import. */
        UINT32 BitDepth = 16;

        /** Creates a new import options object that allows you to customize how are audio clips imported. */
        static SPtr<AudioClipImportOptions> Create();
    };
}
