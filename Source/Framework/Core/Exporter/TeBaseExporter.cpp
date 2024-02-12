#include "Exporter/TeBaseExporter.h"

namespace te
{
    SPtr<ExportOptions> BaseExporter::CreateExportOptions() const
    {
        return te_shared_ptr_new<ExportOptions>();
    }

    SPtr<const ExportOptions> BaseExporter::GetDefaultExportOptions() const
    {
        if (_defaultExportOptions == nullptr)
            _defaultExportOptions = CreateExportOptions();

        return _defaultExportOptions;
    }
}
