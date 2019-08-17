#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"

namespace te
{
    /** Module responsible for importing various asset types and converting them to types usable by the engine. */
	class TE_CORE_EXPORT Importer : public Module<Importer>
	{
	public:
		Importer() = default; 
		~Importer() = default; 
    };
}