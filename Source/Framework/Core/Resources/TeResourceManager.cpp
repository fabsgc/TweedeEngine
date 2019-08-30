#include "Resources/TeResourceManager.h"
#include "Resources/TeResource.h"

namespace te
{
    ResourceManager::ResourceManager()
    {}

    ResourceManager::~ResourceManager()
    {
        UnloadAll();
    }

    HResource ResourceManager::Load(const String& filePath)
    {
        UUID uuid;
        bool foundUUID = GetUUIDFromFile(filePath, uuid);

        if (!foundUUID)
        {
            uuid = UUIDGenerator::GenerateRandom();
        }
            
        return LoadInternal(uuid, filePath);
    }

    void ResourceManager::Update(HResource& handle, const SPtr<Resource>& resource)
    {
        OnResourceModified(handle);

        const UUID& uuid = handle.GetUUID();
        handle.SetHandleData(resource, uuid);

        if (resource)
        {
            auto iterFind = _loadedResources.find(uuid);
            if (iterFind == _loadedResources.end())
            {
                LoadedResourceData& resData = _loadedResources[uuid];

                resData.Resource._data->resource = handle.GetHandleData()->resource;
                resData.Resource._data->uuid = handle.GetHandleData()->uuid;
            }
        }

        OnResourceModified(handle);
    }

    void ResourceManager::Release(ResourceHandleBase& resource)
    {
        resource.RemoveInternalRef();

        if (resource.GetHandleData()->refCount == 0)
        {
            Destroy(resource);
        }
    }

    void ResourceManager::UnloadAllUnused()
    {
        Vector<HResource> resourcesToUnload;

        for (auto iter = _loadedResources.begin(); iter != _loadedResources.end(); ++iter)
        {
            const LoadedResourceData& resData = iter->second;
            UINT32 refCount = resData.Resource.GetHandleData()->refCount;

            assert(refCount > 0);

            if (refCount == resData.InternalRefCount) // Only internal references exist, free it
            {
                resourcesToUnload.push_back(resData.Resource);
            }
        }

        // Note: When unloading multiple resources it's possible that unloading one will also unload
        // another resource in "resourcesToUnload". This is fine because "unload" deals with invalid
        // handles gracefully.
        for (auto iter = resourcesToUnload.begin(); iter != resourcesToUnload.end(); ++iter)
        {
            Release(*iter);
        }
    }

    void ResourceManager::UnloadAll()
    {
        for (auto& loadedResourcePair : _loadedResources)
        {
            Destroy(loadedResourcePair.second.Resource);
        } 
    }

    void ResourceManager::Destroy(ResourceHandleBase& resource)
    {
        if (resource._data == nullptr)
        {
            return;
        }

        const UUID& uuid = resource.GetUUID();
        OnResourceDestroyed(uuid);

        resource._data->resource.reset();

        {
            auto iterFind = _loadedResources.find(uuid);
            if (iterFind != _loadedResources.end())
            {
                LoadedResourceData& resData = iterFind->second;
                while (resData.InternalRefCount > 0)
                {
                    resData.InternalRefCount--;
                    resData.Resource.RemoveInternalRef();
                }

                _loadedResources.erase(iterFind);
            }
            else
            {
                // This should never happen but in case it does fail silently in release mode
                TE_ASSERT_ERROR(false, "Trying to destroy an inexisting resource" + uuid.ToString());
            }
        }

        resource.ClearHandleData();
    }

    HResource ResourceManager::LoadInternal(const UUID& uuid, const String& filePath)
    {
        HResource resource;
        OnResourceLoaded(resource);

        // TODO

        return resource;
    }

    bool ResourceManager::GetUUIDFromFile(const String& filePath, UUID& uuid)
    {
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
    }

    void ResourceManager::UnregisterResource(const UUID& uuid)
    {
        auto iterFind = _UUIDToFile.find(uuid);

        if (iterFind != _UUIDToFile.end())
        {
            _fileToUUID.erase(iterFind->second);
            _UUIDToFile.erase(uuid);
        }
    }

    TE_CORE_EXPORT ResourceManager& gResourceManager()
    {
        return ResourceManager::Instance();
    }
}