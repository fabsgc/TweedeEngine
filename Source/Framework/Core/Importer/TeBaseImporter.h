#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
	/**
	 * Abstract class that needs to be specialized for converting a certain asset type into an engine usable resource
	 * (for example a .png file into an engine usable texture).
	 * 			
	 * On initialization this class must register itself with the Importer module, which delegates asset import calls to a 
	 * specific importer.
	 */
	class TE_CORE_EXPORT BaseImporter
	{
	public:
		BaseImporter() = default;
        virtual ~BaseImporter() = 0;
    };
}