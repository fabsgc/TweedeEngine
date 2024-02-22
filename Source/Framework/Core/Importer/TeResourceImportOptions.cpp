#include "Importer/TeResourceImportOptions.h"

namespace te
{
    ResourceImportOptions::ResourceImportOptions()
        : ImportOptions(TID_ResourceImportOptions)
    { }

    SPtr<ResourceImportOptions> ResourceImportOptions::Create()
    {
        return te_shared_ptr_new<ResourceImportOptions>();
    }
}
