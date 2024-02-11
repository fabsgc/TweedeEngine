#include "Exporter/TeExportOptions.h"

namespace te
{
    ExportOptions::ExportOptions()
        : Serializable(TID_ExportOptions)
    { }

    ExportOptions::ExportOptions(UINT32 type)
        : Serializable(type)
    { }
}
