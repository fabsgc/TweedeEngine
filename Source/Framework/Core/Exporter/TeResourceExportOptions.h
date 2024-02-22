#pragma once

#include "TeCorePrerequisites.h"
#include "Exporter/TeExportOptions.h"

namespace te
{
    /** Contains export options you may use to control how an engine resource is exported. */
    class TE_CORE_EXPORT ResourceExportOptions : public ExportOptions
    {
    public:
        ResourceExportOptions();

        /** Creates a new export options object that allows you to customize how are scenes exported. */
        static SPtr<ResourceExportOptions> Create();
    };
}
