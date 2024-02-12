#include "Importer/TeSceneImportOptions.h"

namespace te
{
    SceneImportOptions::SceneImportOptions()
        : ImportOptions(TID_SceneImportOptions)
    { }

    SPtr<SceneImportOptions> SceneImportOptions::Create()
    {
        return te_shared_ptr_new<SceneImportOptions>();
    }
}
