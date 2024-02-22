#pragma once

#include "TeCorePrerequisites.h"
#include "Exporter/TeExportOptions.h"

namespace te
{
    /** Contains export options you may use to control how a project is exported. */
    class TE_CORE_EXPORT ProjectExportOptions : public ExportOptions
    {
    public:
        ProjectExportOptions();

        /** Creates a new export options object that allows you to customize how are scenes exported. */
        static SPtr<ProjectExportOptions> Create();
    };
}
