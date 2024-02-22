#include "Importer/TeProjectImportOptions.h"

namespace te
{
    ProjectImportOptions::ProjectImportOptions()
        : ImportOptions(TID_ProjectImportOptions)
    { }

    SPtr<ProjectImportOptions> ProjectImportOptions::Create()
    {
        return te_shared_ptr_new<ProjectImportOptions>();
    }
}
