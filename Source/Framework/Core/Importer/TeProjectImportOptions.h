#pragma once

#include "TeCorePrerequisites.h"
#include "Importer/TeImportOptions.h"

namespace te
{
    /** Contains Import options you may use to control how a project is imported. */
    class TE_CORE_EXPORT ProjectImportOptions : public ImportOptions
    {
    public:
        ProjectImportOptions();

        /** Creates a new import options object that allows you to customize how are scenes imported. */
        static SPtr<ProjectImportOptions> Create();
    };
}

