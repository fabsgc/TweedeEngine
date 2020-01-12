#include "Importer/TeShaderImportOptions.h"

namespace te
{
    SPtr<ShaderImportOptions> ShaderImportOptions::Create()
	{
		return te_shared_ptr_new<ShaderImportOptions>();
	}
}