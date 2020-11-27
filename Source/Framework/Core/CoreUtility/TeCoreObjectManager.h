#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"

namespace te
{
    class TE_CORE_EXPORT CoreObjectManager : public Module<CoreObjectManager>
    {
    public:
        CoreObjectManager();
        ~CoreObjectManager();

        /** Generates a new unique ID for a core object. */
        UINT64 GenerateId();

        /** Registers a new CoreObject notifying the manager the object is created. */
        void RegisterObject(CoreObject* object);

        /** Unregisters a CoreObject notifying the manager the object is destroyed. */
        void UnregisterObject(CoreObject* object);

        /**	Notifies the system that a CoreObject is dirty and needs to be synced with the core thread. */
        void NotifyCoreDirty(CoreObject* object);

        /** Synchronize all dirty objects once per frame */
        void FrameSync();

    private:
        UINT64 _nextAvailableID;
        UnorderedMap<UINT64, CoreObject*> _objects;
        UnorderedMap<UINT64, CoreObject*> _dirtyObjects;
    };
}
