#include "Resources/TeResourceManager.h"
#include "Resources/TeResource.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(ResourceManager)

    ResourceManager::ResourceManager()
    {}

    ResourceManager::~ResourceManager()
    {
        UnloadAll();
    }

    void ResourceManager::Release(ResourceHandleBase& resource)
    {
        Destroy(resource);
    }

    void ResourceManager::UnloadAll()
    {
        UnorderedMap<UUID, LoadedResourceData> loadedResourcesCopy = _loadedResources;
        for (auto& loadedResourcePair : loadedResourcesCopy)
        {
            Destroy(loadedResourcePair.second.resource);
        }
    }

    void ResourceManager::Destroy(ResourceHandleBase& resource)
    {
        if (resource._handleData == nullptr)
        {
            return;
        }

        const UUID& uuid = resource.GetUUID();
        OnResourceDestroyed(uuid);
        resource._handleData->data->Destroy();

        auto iterFind = _loadedResources.find(uuid);
        if (iterFind != _loadedResources.end())
        {
            _loadedResources.erase(iterFind);
        }
        else
        {
            // This should never happen but in case it does fail silently in release mode
            TE_ASSERT_ERROR(false, "Trying to destroy an inexisting resource" + uuid.ToString(), __FILE__, __LINE__);
        }

        resource.ClearHandleData();
    }

    void ResourceManager::Update(HResource& handle, const SPtr<Resource>& resource)
    {
        const UUID& uuid = handle.GetUUID();
        handle.SetHandleData(resource, uuid);

        if (resource)
        {
            auto iterFind = _loadedResources.find(uuid);
            if (iterFind == _loadedResources.end())
            {
                LoadedResourceData& resData = _loadedResources[uuid];
                resData.resource = handle.GetNewHandleFromExisting();
            }
        }

        OnResourceModified(handle);
    }

    HResource ResourceManager::Get(const UUID& uuid)
    {
        HResource resource;

        auto iterFind = _loadedResources.find(uuid);
        if (iterFind != _loadedResources.end())
        {
            resource.SetHandleData(iterFind->second.resource.GetHandleData());
        }
        else
        {
            // This should never happen but in case it does fail silently in release mode
            TE_ASSERT_ERROR(false, "Trying to destroy an inexisting resource" + uuid.ToString(), __FILE__, __LINE__);
        }

        return resource;
    }

    bool ResourceManager::GetUUIDFromFile(const String& filePath, UUID& uuid)
    {
        ::RecursiveLock lock(_loadingUuidMutex);

        auto iterFind = _fileToUUID.find(filePath);

        if (iterFind != _fileToUUID.end())
        {
            uuid = iterFind->second;
            return true;
        }
        else
        {
            uuid = UUID::EMPTY;
            return false;
        }
    }

    bool ResourceManager::GetFileFromUUID(const UUID& uuid, String& filePath)
    {
        ::RecursiveLock lock(_loadingUuidMutex);

        auto iterFind = _UUIDToFile.find(uuid);

        if (iterFind != _UUIDToFile.end())
        {
            filePath = iterFind->second;
            return true;
        }
        else
        {
            filePath.clear();
            return false;
        }
    }

    void ResourceManager::RegisterResource(const UUID& uuid, const String& filePath)
    {
        _loadingResourceMutex.lock();

        auto iterFind = _UUIDToFile.find(uuid);

        if (iterFind != _UUIDToFile.end())
        {
            if (iterFind->second != filePath)
            {
                _fileToUUID.erase(iterFind->second);

                _UUIDToFile[uuid] = filePath;
                _fileToUUID[filePath] = uuid;
            }
        }
        else
        {
            auto iterFind2 = _fileToUUID.find(filePath);
            if (iterFind2 != _fileToUUID.end())
            {
                _UUIDToFile.erase(iterFind2->second);
            }

            _UUIDToFile[uuid] = filePath;
            _fileToUUID[filePath] = uuid;
        }

        _loadingResourceMutex.unlock();
    }

    HResource ResourceManager::_createResourceHandle(const SPtr<Resource>& obj)
    {
        UUID uuid = UUIDGenerator::GenerateRandom();
        return _createResourceHandle(obj, uuid);
    }

    HResource ResourceManager::_createResourceHandle(const SPtr<Resource>& obj, const UUID& UUID)
    {
        //_loadingResourceMutex.lock();

        if (UUID.Empty())
        {
            return _createResourceHandle(obj);
        }

        ResourceHandle<Resource> hr = ResourceHandle<Resource>(obj, UUID);

        if (_loadedResources.find(UUID) == _loadedResources.end())
        {
            _loadedResources[UUID] = static_resource_cast<Resource>(hr);
            OnResourceLoaded(Get(UUID));
        }

        //_loadingResourceMutex.unlock();

        return static_resource_cast<Resource>(Get(UUID));;
    }

    TE_CORE_EXPORT ResourceManager& gResourceManager()
    {
        return ResourceManager::Instance();
    }
}
