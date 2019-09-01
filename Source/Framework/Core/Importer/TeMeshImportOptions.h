#pragma once

#include "TeCorePrerequisites.h"
#include "Importer/TeImportOptions.h"

namespace te
{
	/** Contains import options you may use to control how is a Mesh imported. */
	class TE_CORE_EXPORT MeshImportOptions : public ImportOptions
	{
	public:
		MeshImportOptions() = default;

        /** Creates a new import options object that allows you to customize how are Meshs imported. */
		static SPtr<MeshImportOptions> Create();
    };
}
