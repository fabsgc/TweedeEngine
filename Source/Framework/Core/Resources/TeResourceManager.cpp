#include "Resources/TeResourceManager.h"
#include "Resources/TeResource.h"

namespace te
{
    ResourceManager::ResourceManager()
    {}

    ResourceManager::~ResourceManager()
    {}

    HResource ResourceManager::Load(const String& filePath)
    {
        HResource resource;
        return resource;
    }

    void ResourceManager::Release(ResourceHandleBase& resource)
    {
        // TODO
    }

    void ResourceManager::UnloadAllUnused()
    {
        // TODO
    }

    void ResourceManager::UnloadAll()
    {
        // TODO
    }

    void ResourceManager::Destroy(ResourceHandleBase& resource)
    {
        // TODO
    }

    TE_CORE_EXPORT ResourceManager& gResourceManager()
    {
        return ResourceManager::Instance();
    }
}