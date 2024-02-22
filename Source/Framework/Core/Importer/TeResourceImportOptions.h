#pragma once

#include "TeCorePrerequisites.h"
#include "Importer/TeImportOptions.h"

namespace te
{
    /** Contains Import options you may use to control how an engine resource is imported. */
    class TE_CORE_EXPORT ResourceImportOptions : public ImportOptions
    {
    public:
        ResourceImportOptions();

        /** Creates a new import options object that allows you to customize how are scenes imported. */
        static SPtr<ResourceImportOptions> Create();
    };
}

