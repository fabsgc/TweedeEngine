#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"
#include "Resources/TeResourceHandle.h"

namespace te
{
    /** Manager that handles resource loading. */
	class TE_CORE_EXPORT ResourceManager: public Module<ResourceManager>
	{
	public:
		ResourceManager();
		~ResourceManager();

    private:
        UnorderedMap<UUID, TResourceHandle<Resource>> _handles;
    };
}