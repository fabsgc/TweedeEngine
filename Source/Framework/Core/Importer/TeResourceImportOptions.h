#pragma once

#include "TeCorePrerequisites.h"
#include "Importer/TeImportOptions.h"

namespace te
{
    /** Contains Import options you may use to control how an engine resource is imported. */
    class TE_CORE_EXPORT ResourceImportOptions : public ImportOptions
    {
    public:
        ResourceImportOptions() = default;
        ~ResourceImportOptions() = default;

        CoreType ResourceType = CoreType::TID_Unknown;
    };
}

