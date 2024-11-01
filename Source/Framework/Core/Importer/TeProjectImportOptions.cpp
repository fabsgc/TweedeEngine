#include "Importer/TeProjectImportOptions.h"

namespace te
{
    ProjectImportOptions::ProjectImportOptions()
    { }

    SPtr<ProjectImportOptions> ProjectImportOptions::Create()
    {
        return te_shared_ptr_new<ProjectImportOptions>();
    }
}
