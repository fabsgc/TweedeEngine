#include "Importer/TeTextureImportOptions.h"

namespace te
{
    SPtr<TextureImportOptions> TextureImportOptions::Create()
	{
		return te_shared_ptr_new<TextureImportOptions>();
	}
}