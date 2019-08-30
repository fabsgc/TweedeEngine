#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
	/**
	 * Base class for creating import options from. Import options are specific for each importer and control how is data
	 * imported.
	 */
	class TE_CORE_EXPORT ImportOptions
	{
	public:
		virtual ~ImportOptions() = default;
	};
}
