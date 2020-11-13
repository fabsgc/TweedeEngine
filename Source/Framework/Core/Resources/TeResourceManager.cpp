#include "Resources/TeResourceManager.h"
#include "Resources/TeResource.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(ResourceManager)

    ResourceManager::ResourceManager()
    {
    }

    ResourceManager::~ResourceManager()
    {
        UnloadAll();
    }

    void ResourceManager::Release(ResourceHandleBase& resource)
    {
        UnregisterResource(resource.GetUUID());
        Destroy(resource);
    }

    void ResourceManager::UnloadAll()
    {
        UnorderedMap<UUID, LoadedResourceData> loadedResourcesCopy = _loadedResources;
        for (auto& loadedResourcePair : loadedResourcesCopy)
        {
            UnregisterResource(loadedResourcePair.second.resource.GetUUID());
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
            TE_ASSERT_ERROR(false, "Trying to destroy an inexisting resource : " + uuid.ToString());
        }

        resource.ClearHandleData();
    }

    SPtr<MultiResource> ResourceManager::LoadAll(const String& filePath, const SPtr<const ImportOptions>& options)
    {
        UUID uuid;
        SPtr<MultiResource> resources;
        GetUUIDFromFile(filePath, uuid);

        if (uuid.Empty())
        {
            Vector<SubResourceUUID> subResourcesUUID;
            resources = gImporter().ImportAll(filePath, options);

            if (resources->Entries.size() > 0)
            {
                for (auto& entry : resources->Entries)
                {
                    UUID resourceUuid = entry.Res.GetUUID();
                    entry.Res.GetInternalPtr()->_UUID = resourceUuid;

                    if (entry.Name == "primary")
                    {
                        uuid = resourceUuid;
                        RegisterResource(resourceUuid, filePath);
                        _loadedResources[resourceUuid] = entry.Res;
                    }
                    else
                    {
                        _loadedResources[resourceUuid] = entry.Res;
                        subResourcesUUID.push_back({ entry.Name, resourceUuid });
                    }
                }

                _resourcesChunks[uuid] = subResourcesUUID;
            }
        }
        else
        {
            if (_resourcesChunks.find(uuid) != _resourcesChunks.end())
            {
                resources = te_shared_ptr_new<MultiResource>();
                Vector<SubResourceUUID> subResourcesUuid = _resourcesChunks[uuid];

                resources->Entries.push_back({ "primary", Get(uuid) });

                for (auto& subRes : subResourcesUuid)
                {
                    HResource res = Get(subRes.Uuid);
                    if (res.GetHandleData())
                        resources->Entries.push_back({ subRes.Name, res });
                }
            }
            else
            {
                HResource res = Get(uuid);
                if (res.GetHandleData())
                    resources->Entries.push_back({ "primary", res });
            }
        }

        return resources;
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
            TE_DEBUG("Resource not found : " + uuid.ToString());
        }

        return resource;
    }

    Vector<HResource> ResourceManager::GetAll()
    {
        Vector<HResource> resources;
        HResource resource;

        for (auto& item : _loadedResources)
        {
            resource = item.second.resource.GetNewHandleFromExisting();
            resources.push_back(resource);
        }

        return resources;
    }

    Vector<HResource> ResourceManager::FindByType(UINT32 type)
    {
        Vector<HResource> resources;
        HResource resource;

        for (auto& item : _loadedResources)
        {
            if (item.second.resource->GetCoreType() == type)
            {
                resource = item.second.resource.GetNewHandleFromExisting();
                resources.push_back(resource);
            }
            
        }

        return resources;
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

    void ResourceManager::UnregisterResource(const UUID& uuid)
    {
        _loadingResourceMutex.lock();

        auto iterChunkUUID = _resourcesChunks.find(uuid);
        if (iterChunkUUID != _resourcesChunks.end())
        {
            for (auto& subResource : _resourcesChunks[uuid])
                Release(subResource.Uuid);

            _resourcesChunks.erase(iterChunkUUID);
        }

        auto iterUUID = _UUIDToFile.find(uuid);
        if (iterUUID != _UUIDToFile.end())
        {
            _UUIDToFile.erase(iterUUID);
        }

        for(auto iterFile = _fileToUUID.begin(); iterFile != _fileToUUID.end(); iterFile++)
        {
            if(iterFile->second == uuid)
            {
                _fileToUUID.erase(iterFile);
                break;
            }
        }

        _loadingResourceMutex.unlock();
    }

    HResource ResourceManager::_createResourceHandle(const SPtr<Resource>& obj)
    {
        UUID uuid = UUIDGenerator::GenerateRandom();
        obj->_setUUID(uuid);
        return _createResourceHandle(obj, uuid);
    }

    HResource ResourceManager::_createResourceHandle(const SPtr<Resource>& obj, const UUID& UUID)
    {
        if (UUID.Empty())
        {
            return _createResourceHandle(obj);
        }

        ResourceHandle<Resource> hr = ResourceHandle<Resource>(obj, UUID);

        if (_loadedResources.find(UUID) == _loadedResources.end())
        {
            _loadingResourceMutex.lock();
            _loadedResources[UUID] = static_resource_cast<Resource>(hr);
            _loadingResourceMutex.unlock();
            OnResourceLoaded(Get(UUID));
        }

        return static_resource_cast<Resource>(Get(UUID));
    }

    TE_CORE_EXPORT ResourceManager& gResourceManager()
    {
        return ResourceManager::Instance();
    }
}
