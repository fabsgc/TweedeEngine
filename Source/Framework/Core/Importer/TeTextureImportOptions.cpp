#include "Importer/TeTextureImportOptions.h"

namespace te
{
    TextureImportOptions::TextureImportOptions()
        : ImportOptions(TID_TextureImportOptions)
    { }

    SPtr<TextureImportOptions> TextureImportOptions::Create()
    {
        return te_shared_ptr_new<TextureImportOptions>();
    }
}
