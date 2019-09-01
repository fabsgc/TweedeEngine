#pragma once

#include "TeCorePrerequisites.h"
#include "Importer/TeImportOptions.h"

namespace te
{
	/** Contains import options you may use to control how is a texture imported. */
	class TE_CORE_EXPORT TextureImportOptions : public ImportOptions
	{
	public:
		TextureImportOptions() = default;

        /** Creates a new import options object that allows you to customize how are textures imported. */
		static SPtr<TextureImportOptions> Create();
    };
}