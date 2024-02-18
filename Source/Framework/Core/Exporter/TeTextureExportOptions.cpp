#include "Exporter/TeTextureExportOptions.h"

namespace te
{
    TextureExportOptions::TextureExportOptions()
        : ExportOptions(TID_TextureExportOptions)
    { }

    SPtr<TextureExportOptions> TextureExportOptions::Create()
    {
        return te_shared_ptr_new<TextureExportOptions>();
    }
}
