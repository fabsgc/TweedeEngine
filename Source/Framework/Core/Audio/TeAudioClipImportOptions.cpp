#include "Audio/TeAudioClipImportOptions.h"

namespace te
{
    AudioClipImportOptions::AudioClipImportOptions()
        : ImportOptions(TID_AudioClipImportOptions)
    { }

    SPtr<AudioClipImportOptions> AudioClipImportOptions::Create()
    {
        return te_shared_ptr_new<AudioClipImportOptions>();
    }
}
