#include "Exporter/TeResourceExportOptions.h"

namespace te
{
    ResourceExportOptions::ResourceExportOptions()
        : ExportOptions(TID_ResourceExportOptions)
    { }

    SPtr<ResourceExportOptions> ResourceExportOptions::Create()
    {
        return te_shared_ptr_new<ResourceExportOptions>();
    }
}
