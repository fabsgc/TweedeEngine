#include "Exporter/TeSceneExportOptions.h"

namespace te
{
    SceneExportOptions::SceneExportOptions()
        : ExportOptions(TID_SceneExportOptions)
    { }

    SPtr<SceneExportOptions> SceneExportOptions::Create()
    {
        return te_shared_ptr_new<SceneExportOptions>();
    }
}
