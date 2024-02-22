#include "Exporter/TeProjectExportOptions.h"

namespace te
{
    ProjectExportOptions::ProjectExportOptions()
        : ExportOptions(TID_ProjectExportOptions)
    { }

    SPtr<ProjectExportOptions> ProjectExportOptions::Create()
    {
        return te_shared_ptr_new<ProjectExportOptions>();
    }
}
