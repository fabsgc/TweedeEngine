#pragma once

#include "TeCorePrerequisites.h"
#include "Exporter/TeExportOptions.h"
#include "Exporter/TeBaseExporter.h"
#include "Utility/TeModule.h"

namespace te
{
    /** Module responsible for exporting various asset types used internally in the engine. */
    class TE_CORE_EXPORT Exporter : public Module<Exporter>
    {
    public:
        Exporter(); 
        virtual ~Exporter();

        TE_MODULE_STATIC_HEADER_MEMBER(Exporter)
    };
}
