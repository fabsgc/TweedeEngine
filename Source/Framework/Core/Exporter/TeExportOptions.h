#pragma once

#include "TeCorePrerequisites.h"
#include "Serialization/TeSerializable.h"

namespace te
{
    /**
     * Base class for creating import options from. Import options are specific for each importer and control how is data
     * imported.
     */
    class TE_CORE_EXPORT ExportOptions : public Serializable
    {
    public:
        ExportOptions();
        ExportOptions(UINT32 type);
        virtual ~ExportOptions() = default;
    };
}
