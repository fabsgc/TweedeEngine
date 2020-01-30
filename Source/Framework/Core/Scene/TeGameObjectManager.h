#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"
#include "Scene/TeGameObject.h"
#include "Utility/TeEvent.h"

namespace te
{
    /**
     * Tracks GameObject creation and destructions. Also resolves GameObject references from GameObject handles.
     */
    class TE_CORE_EXPORT GameObjectManager : public Module<GameObjectManager>
    {
    public:
        GameObjectManager() = default;
        ~GameObjectManager();

        /**
         * Registers a new GameObject and returns the handle to the object.
         *
         * @param[in]	object			Constructed GameObject to wrap in the handle and initialize.
         * @return						Handle to the GameObject.
         */
        GameObjectHandleBase RegisterObject(const SPtr<GameObject>& object);

        /**
         * Unregisters a GameObject. Handles to this object will no longer be valid after this call. This should be called
         * whenever a GameObject is destroyed.
         */
        void UnregisterObject(GameObjectHandleBase& object);

        /**
         * Attempts to find a GameObject handle based on the GameObject instance ID. Returns empty handle if ID cannot be
         * found.
         */
        GameObjectHandleBase GetObjectHandle(UINT64 id) const;

        /**
         * Attempts to find a GameObject handle based on the GameObject instance ID. Returns true if object with the
         * specified ID is found, false otherwise.
         *
         * @note	Thread safe.
         */
        bool TryGetObjectHandle(UINT64 id, GameObjectHandleBase& object) const;

        /**
         * Checks if the GameObject with the specified instance ID exists.
         *
         * @note	Thread safe.
         */
        bool ObjectExists(UINT64 id) const;

        /**
         * Changes the instance ID by which an object can be retrieved by.
         *
         * @note	Caller is required to update the object itself with the new ID.
         * @note	Thread safe.
         */
        void RemapId(UINT64 oldId, UINT64 newId);

        /**
         * Allocates a new unique game object ID.
         *
         * @note	Thread safe.
         */
        UINT64 ReserveId();

        /**	Queues the object to be destroyed at the end of a GameObject update cycle. */
        void QueueForDestroy(const GameObjectHandleBase& object);

        /**	Destroys any GameObjects that were queued for destruction. */
        void DestroyQueuedObjects();

        /**	Triggered when a game object is being destroyed. */
        Event<void(const HGameObject&)> OnDestroyed;

    private:
        std::atomic<UINT64> _nextAvailableID = { 1 }; // 0 is not a valid ID
        Map<UINT64, GameObjectHandleBase> _objects;
        Map<UINT64, GameObjectHandleBase> _queuedForDestroy;
    };
}
