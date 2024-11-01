#include "Exporter/TeProjectExportOptions.h"

namespace te
{
    ProjectExportOptions::ProjectExportOptions()
    { }

    SPtr<ProjectExportOptions> ProjectExportOptions::Create()
    {
        return te_shared_ptr_new<ProjectExportOptions>();
    }
}
