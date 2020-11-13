#include "Importer/TeShaderImportOptions.h"

namespace te
{
    ShaderImportOptions::ShaderImportOptions()
        : ImportOptions(TID_ShaderImportOptions)
    { }

    SPtr<ShaderImportOptions> ShaderImportOptions::Create()
    {
        return te_shared_ptr_new<ShaderImportOptions>();
    }
}
