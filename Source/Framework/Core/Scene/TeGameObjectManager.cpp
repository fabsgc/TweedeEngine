#include "Scene/TeGameObjectManager.h"
#include "Scene/TeGameObject.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(GameObjectManager)

    GameObjectManager::~GameObjectManager()
    {
        DestroyQueuedObjects();
    }

    GameObjectHandleBase GameObjectManager::GetObjectHandle(UINT64 id) const
    {
        const auto iterFind = _objects.find(id);
        if (iterFind != _objects.end())
            return iterFind->second;

        return nullptr;
    }

    bool GameObjectManager::TryGetObjectHandle(UINT64 id, GameObjectHandleBase& object) const
    {
        const auto iterFind = _objects.find(id);
        if (iterFind != _objects.end())
        {
            object = iterFind->second;
            return true;
        }

        return false;
    }

    bool GameObjectManager::ObjectExists(UINT64 id) const
    {
        return _objects.find(id) != _objects.end();
    }

    void GameObjectManager::RemapId(UINT64 oldId, UINT64 newId)
    {
        if (oldId == newId)
            return;

        _objects[newId] = _objects[oldId];
        _objects.erase(oldId);
    }

    UINT64 GameObjectManager::ReserveId()
    {
        return _nextAvailableID.fetch_add(1, std::memory_order_relaxed);
    }

    void GameObjectManager::QueueForDestroy(const GameObjectHandleBase& object)
    {
        if (object.IsDestroyed())
            return;

        const UINT64 instanceId = object->GetInstanceId();
        _queuedForDestroy[instanceId] = object;
    }

    void GameObjectManager::DestroyQueuedObjects()
    {
        for (auto& objPair : _queuedForDestroy)
            objPair.second->DestroyInternal(objPair.second, true);

        _queuedForDestroy.clear();
    }

    GameObjectHandleBase GameObjectManager::RegisterObject(const SPtr<GameObject>& object)
    {
        const UINT64 id = _nextAvailableID.fetch_add(1, std::memory_order_relaxed);
        object->Initialize(object, id);

        GameObjectHandleBase handle(object);
        {
            _objects[id] = handle;
        }

        return handle;
    }

    void GameObjectManager::UnregisterObject(GameObjectHandleBase& object)
    {
        _objects.erase(object->GetInstanceId());
        OnDestroyed(static_object_cast<GameObject>(object));
        object.Destroy();
    }
}
