#include "Importer/TeMeshImportOptions.h"

namespace te
{
    MeshImportOptions::MeshImportOptions()
        : ImportOptions(TID_MeshImportOptions)
    { }

    SPtr<MeshImportOptions> MeshImportOptions::Create()
    {
        return te_shared_ptr_new<MeshImportOptions>();
    }
}
