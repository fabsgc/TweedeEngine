#include "Audio/TeAudioClipImportOptions.h"

namespace te
{
    AudioClipImportOptions::AudioClipImportOptions()
    { }

    SPtr<AudioClipImportOptions> AudioClipImportOptions::Create()
    {
        return te_shared_ptr_new<AudioClipImportOptions>();
    }
}
