#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"
#include "Utility/TeEvent.h"

namespace te
{
    /** Manager that handles resource loading. */
	class TE_CORE_EXPORT ResourceManager: public Module<ResourceManager>
	{
        struct LoadedResourceData
        {
            ResourceHandle<Resource> resource;
            UINT32 InternalRefCount = 0;
        };

	public:
		ResourceManager();
		~ResourceManager();

        HResource Load(const String& filePath);

        void Update(HResource& handle, const SPtr<Resource>& resource);

        template <class T>
        ResourceHandle<T> Load(const String& filePath)
        {
            return static_resource_cast<T>(Load(filePath));
        }

        void release(const HResource& resource) 
        { 
            Release((ResourceHandleBase&)resource); 
        }

        /** @copydoc release(const HResource&) */
        void Release(ResourceHandleBase& resource);

        /**
         * Finds all resources that aren't being referenced outside of the resources system and unloads them.
         * @see	release(const HResource&)
         */
        void UnloadAllUnused();

        /** Forces unload of all resources, whether they are being used or not. */
        void UnloadAll();

        /**	Destroys a resource, freeing its memory. */
        void Destroy(ResourceHandleBase& resource);

    public:
        Event<void(const HResource&)> OnResourceLoaded;

        /** Called when the resource has been destroyed. Provides UUID of the destroyed resource.*/
        Event<void(const UUID&)> OnResourceDestroyed;

        /** Called when the internal resource the handle is pointing to has changed. */
        Event<void(const HResource&)> OnResourceModified;

    private:
        friend class ResourceHandleBase;

    private:
        HResource LoadInternal(const UUID& uuid, const String& filePath);

        bool GetUUIDFromFile(const String& filePath, UUID& uuid);
        bool GetFileFromUUID(const UUID& uuid, String& filePath);
        void RegisterResource(const UUID& uuid, const String& filePath);
        void UnregisterResource(const UUID& uuid);

    private:
        UnorderedMap<UUID, TResourceHandle<Resource>> _handles;
        UnorderedMap<UUID, LoadedResourceData> _loadedResources;
        UnorderedMap<UUID, String> _UUIDToFile;
        UnorderedMap<String, UUID> _fileToUUID;
    };

    TE_CORE_EXPORT ResourceManager& gResourceManager();
}