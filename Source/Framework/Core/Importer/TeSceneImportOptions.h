#pragma once

#include "TeCorePrerequisites.h"
#include "Importer/TeImportOptions.h"

namespace te
{
    /** Contains Import options you may use to control how is a scene Imported. */
    class TE_CORE_EXPORT SceneImportOptions : public ImportOptions
    {
    public:
        SceneImportOptions();

        /** Creates a new import options object that allows you to customize how are scenes imported. */
        static SPtr<SceneImportOptions> Create();
    };
}

