#include "Resources/TeResourceManager.h"
#include "Resources/TeResource.h"

#include <filesystem>

namespace te
{
    TE_MODULE_STATIC_MEMBER(ResourceManager)

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
            Release(loadedResourcePair.second.resource);
        }
    }

    void ResourceManager::Destroy(ResourceHandleBase& resource)
    {
        if (resource._handleData == nullptr || resource._handleData->data == nullptr)
        {
            return;
        }

        resource._handleData->data->Destroy();
        const UUID uuid = resource.GetUUID();
        const CoreType type = resource._handleData->data->GetCoreType();
        OnResourceDestroyed(uuid, type);

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

    SPtr<MultiResource> ResourceManager::LoadAll(const std::filesystem::path& filePath, const ImportOptions& options, LoadingMode mode)
    {
        UUID uuid;
        SPtr<MultiResource> resources;
        GetUUIDFromFile(filePath, uuid);

        if (uuid.Empty() || mode == LoadingMode::Force || mode == LoadingMode::Replace)
        {
            resources = gImporter().ImportAll(filePath, options);

            if (mode == LoadingMode::Replace && !uuid.Empty())
            {
                Vector<SubResourceUUID>& subResourcesUUID = _resourcesChunks[uuid];
                auto currentSubResource = subResourcesUUID.begin();

                HResource existingResource = Get(uuid);
                TE_ASSERT_ERROR(existingResource.IsLoaded(), "Resource not loaded for UUID: " + uuid.ToString());

                for (auto& entry : resources->Entries)
                {
                    if (entry.Name == "primary")
                    {
                        Update(existingResource, entry.Res.GetInternalPtr());
                    }
                    else if (entry.Res.IsLoaded())
                    {
                        if (currentSubResource != subResourcesUUID.end())
                        {
                            HResource existingSubResource = Get(currentSubResource->Uuid);
                            TE_ASSERT_ERROR(existingSubResource.IsLoaded(), "Sub resource not loaded for UUID: " + currentSubResource->Uuid.ToString());

                            if (entry.Res.Get()->GetCoreType() == existingSubResource.Get()->GetCoreType())
                            {
                                Update(existingSubResource, entry.Res.GetInternalPtr());
                                currentSubResource++;
                                continue;
                            }
                        }

                        const UUID& resourceUuid = entry.Res.GetUUID();
                        _loadedResources[resourceUuid] = entry.Res;
                        subResourcesUUID.push_back({ entry.Name, resourceUuid });

                        currentSubResource++;
                    }
                }
            }
            else
            {
                Vector<SubResourceUUID> subResourcesUUID;

                for (auto& entry : resources->Entries)
                {
                    const UUID& resourceUuid = entry.Res.GetUUID();
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
            HResource res = Get(uuid);
            TE_ASSERT_ERROR(res.IsLoaded(), "Primary resource not loaded for UUID: " + uuid.ToString());

            resources = te_shared_ptr_new<MultiResource>();
            resources->Entries.push_back({ "primary", res });

            for (auto& subElement : _resourcesChunks[uuid])
            {
                HResource subRes = Get(subElement.Uuid);
                TE_ASSERT_ERROR(res.IsLoaded(), "Sub resource not loaded for UUID: " + subElement.Uuid.ToString());
                resources->Entries.push_back({ subElement.Name, subRes });
            }
        }

        return resources;
    }

    void ResourceManager::Update(HResource& handle, SPtr<Resource> resource)
    {
        const UUID& uuid = handle.GetUUID();

        resource->SetUUID(uuid);
        handle.SetHandleData(resource, uuid);

        if (resource)
        {
            auto iterFind = _loadedResources.find(uuid);
            if (iterFind == _loadedResources.end())
            {
                LoadedResourceData& resData = _loadedResources[uuid];
                resData.resource = handle;
            }
        }

        OnResourceModified(handle);
    }

    HResource ResourceManager::Get(const UUID& uuid)
    {
        HResource resource;
        RecursiveLock lock(_loadingResourceMutex);

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
            resource = item.second.resource;
            resources.push_back(resource);
        }

        return resources;
    }

    Vector<HResource> ResourceManager::FindByType(CoreType type)
    {
        Vector<HResource> resources;
        HResource resource;

        for (auto& item : _loadedResources)
        {
            if (item.second.resource->GetCoreType() == type)
            {
                resource = item.second.resource;
                resources.push_back(resource);
            }
            
        }

        return resources;
    }

    bool ResourceManager::GetUUIDFromFile(const std::filesystem::path& filePath, UUID& uuid)
    {
        std::error_code e;
        const auto absolutePath = std::filesystem::weakly_canonical(filePath, e).generic_string();

        RecursiveLock lock(_loadingUuidMutex);

        auto iterFind = _fileToUUID.find(absolutePath);

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
        RecursiveLock lock(_loadingUuidMutex);

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

    void ResourceManager::RegisterResource(const UUID& uuid, const std::filesystem::path& filePath)
    {
        std::error_code e;
        const auto absolutePath = std::filesystem::weakly_canonical(filePath, e).generic_string();

        RecursiveLock lock(_loadingResourceMutex);

        const auto iterFind = _UUIDToFile.find(uuid);
        if (iterFind != _UUIDToFile.end())
        {
            if (iterFind->second != absolutePath)
            {
                _fileToUUID.erase(iterFind->second);

                _UUIDToFile[uuid] = absolutePath;
                _fileToUUID[absolutePath] = uuid;
            }
        }
        else
        {
            const auto iterFind2 = _fileToUUID.find(absolutePath);
            if (iterFind2 != _fileToUUID.end())
            {
                _UUIDToFile.erase(iterFind2->second);
            }

            _UUIDToFile[uuid] = absolutePath;
            _fileToUUID[absolutePath] = uuid;
        }
    }

    void ResourceManager::UnregisterResource(const UUID& uuid)
    {
        RecursiveLock lock(_loadingResourceMutex);

        auto iterChunkUUID = _resourcesChunks.find(uuid);
        if (iterChunkUUID != _resourcesChunks.end())
        {
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
    }

    HResource ResourceManager::_createResourceHandle(const SPtr<Resource>& obj)
    {
        obj->SetUUID(UUIDGenerator::GenerateRandom());
        return _createResourceHandle(obj, obj->GetUUID());
    }

    HResource ResourceManager::_createResourceHandle(const SPtr<Resource>& obj, const UUID& UUID)
    {
        if (UUID.Empty() && obj->GetUUID().Empty())
        {
            return _createResourceHandle(obj);
        }
        else if (!UUID.Empty())
        {
            obj->SetUUID(UUID);
        }

        ResourceHandle<Resource> hr = ResourceHandle<Resource>(obj, obj->GetUUID());

        if (_loadedResources.find(obj->GetUUID()) == _loadedResources.end())
        {
            {
                RecursiveLock lock(_loadingResourceMutex);
                _loadedResources[obj->GetUUID()] = static_resource_cast<Resource>(hr);
            }

            OnResourceLoaded(Get(obj->GetUUID()));
        }

        return static_resource_cast<Resource>(Get(obj->GetUUID()));
    }

    TE_CORE_EXPORT ResourceManager& gResourceManager()
    {
        return ResourceManager::Instance();
    }
}
