#pragma once

#include "TeCorePrerequisites.h"
#include "Exporter/TeExportOptions.h"

namespace te
{
    /** Contains export options you may use to control how is a scene exported. */
    class TE_CORE_EXPORT SceneExportOptions : public ExportOptions
    {
    public:
        SceneExportOptions();

        /** Creates a new export options object that allows you to customize how are scenes exported. */
        static SPtr<SceneExportOptions> Create();
    };
}
