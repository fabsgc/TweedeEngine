#pragma once

#include "TeCorePrerequisites.h"
#include "Importer/TeImportOptions.h"

namespace te
{
    /** Contains import options you may use to control how an audio clip is imported. */
    class TE_CORE_EXPORT AudioClipImportOptions : public ImportOptions
    {
    public:
        AudioClipImportOptions() = default;

        /** Creates a new import options object that allows you to customize how are audio clips imported. */
        static SPtr<AudioClipImportOptions> Create();
    };
}
