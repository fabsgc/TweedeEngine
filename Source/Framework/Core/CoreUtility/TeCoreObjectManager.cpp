#include "TeCoreObjectManager.h"
#include "TeCoreObject.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(CoreObjectManager)

    CoreObjectManager::CoreObjectManager()
        :_nextAvailableID(1)
    { }

    CoreObjectManager::~CoreObjectManager()
    {
#if TE_DEBUG_MODE
        if(_objects.size() > 0)
        {
            // All objects MUST be destroyed at this point, otherwise there might be memory corruption.
            // (Reason: This is called on application shutdown and at that point we also unload any dynamic libraries,
            // which will invalidate any pointers to objects created from those libraries. Therefore we require of the user to
            // clean up all objects manually before shutting down the application).
            TE_ASSERT_ERROR(false, "Core object manager shut down, but not all objects were released. Application must release ALL engine objects before shutdown.", __FILE__, __LINE__);
        }
#endif
    }

    UINT64 CoreObjectManager::GenerateId()
    {
        return _nextAvailableID++;
    }

    void CoreObjectManager::RegisterObject(CoreObject* object)
    {
        UINT64 objId = object->GetInternalID();
        _objects[objId] = object;
    }

    void CoreObjectManager::UnregisterObject(CoreObject* object)
    {
        assert(object != nullptr && !object->IsDestroyed());
        UINT64 internalId = object->GetInternalID();
        _objects.erase(internalId);
    }
}
