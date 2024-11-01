#include "Exporter/TeResourceExportOptions.h"

namespace te
{
    ResourceExportOptions::ResourceExportOptions()
    { }

    SPtr<ResourceExportOptions> ResourceExportOptions::Create()
    {
        return te_shared_ptr_new<ResourceExportOptions>();
    }
}
