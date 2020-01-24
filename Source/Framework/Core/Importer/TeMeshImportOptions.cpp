#include "Importer/TeMeshImportOptions.h"

namespace te
{
    SPtr<MeshImportOptions> MeshImportOptions::Create()
    {
        return te_shared_ptr_new<MeshImportOptions>();
    }
}