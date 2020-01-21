#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"

namespace te
{
	class CoreObjectManager : public Module<CoreObjectManager>
	{
	public:
		CoreObjectManager();
		~CoreObjectManager();

		/** Generates a new unique ID for a core object. */
		UINT64 GenerateId();

		/** Registers a new CoreObject notifying the manager the object	is created. */
		void RegisterObject(CoreObject* object);

		/** Unregisters a CoreObject notifying the manager the object is destroyed. */
		void UnregisterObject(CoreObject* object);

	private:
		UINT64 _nextAvailableID;
		Map<UINT64, CoreObject*> _objects;
	};
}

