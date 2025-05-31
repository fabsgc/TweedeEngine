#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"
#include "Utility/TeEvent.h"
#include "Importer/TeImporter.h"
#include "Threading/TeThreading.h"

namespace te
{
    /** Manager that handles resource loading.*/
    class TE_CORE_EXPORT ResourceManager: public Module<ResourceManager>
    {
        struct LoadedResourceData
        {
            HResource resource;

            LoadedResourceData() = default;
            LoadedResourceData(const HResource& resource)
                : resource(resource)
            {}
        };

    public:
        enum class LoadingMode : uint32_t
        {
            Force = 0x1, /** Force to load the resource even if it is already loaded, create a new resource */
            Replace = 0x2, /** Force to load the resource even if it is already loaded, replace the existing resource */
            KeepExisting = 0x3 /** Keep the existing resource if it is already loaded, return a handle to the already existing one */
        };

    public:
        ResourceManager() = default;

        virtual ~ResourceManager();

        TE_MODULE_STATIC_HEADER_MEMBER(ResourceManager)

    public:
        /** Forces to unload all resources, whether they are being used or not. */
        void UnloadAll();

        /** Releases a resource, by unregistering it from the manager and freeing its memory. */
        void Release(const HResource& resource)
        {
            Release((ResourceHandleBase&)resource);
        }

        /** @copydoc ResourceManager::Release */
        void Release(ResourceHandleBase& resource);

    public:
        template <class T>
        ResourceHandle<T> Load(const std::filesystem::path& filePath, const ImportOptions& options, LoadingMode mode = LoadingMode::KeepExisting)
        {
            UUID uuid;
            ResourceHandle<T> resourceHandle;
            GetUUIDFromFile(filePath, uuid);

            if (uuid.Empty() || mode == LoadingMode::Force || mode == LoadingMode::Replace)
            {
                resourceHandle = gImporter().Import<T>(filePath, options);
                TE_ASSERT_ERROR(resourceHandle.IsLoaded(), "Resource not loaded for UUID: " + uuid.ToString());

                if (resourceHandle.IsLoaded())
                {
                    if (mode == LoadingMode::Replace && !uuid.Empty())
                    {
                        HResource existingResource = Get(uuid);
                        Update(existingResource, resourceHandle.GetInternalPtr());
                    }
                    else
                    {
                        uuid = resourceHandle.GetUUID();
                        resourceHandle.GetInternalPtr()->_UUID = uuid;
                        RegisterResource(uuid, filePath);
                        _loadedResources[uuid] = static_resource_cast<Resource>(resourceHandle);
                    }

                    return static_resource_cast<T>(Get(uuid));
                }
                else
                {
                    return resourceHandle;
                }
            }

            return static_resource_cast<T>(Get(uuid));
        }

        /**
         * By using this importer, because non primary resources are not linked to a file, we need to 
         * find associated subResources and return a MultiResource instance
        */
        SPtr<MultiResource> LoadAll(const std::filesystem::path& filePath, const ImportOptions& options, LoadingMode loadingMode = LoadingMode::KeepExisting);

        /** Updates the internal resource the handle is pointing to. */
        void Update(HResource& handle, SPtr<Resource> resource);

    public:
        /** Creates a new resource handle from a resource pointer. */
        HResource _createResourceHandle(const SPtr<Resource>& obj);

        /** Creates a new resource handle from a resource pointer, with a user defined UUID. */
        HResource _createResourceHandle(const SPtr<Resource>& obj, const UUID& UUID);

    public:
        template <class T>
        ResourceHandle<T> Get(const UUID& uuid = UUID::EMPTY)
        {
            return static_resource_cast<T>(Get(uuid));
        }

        /** Allows to retrieve a resource using its uuid */
        HResource Get(const UUID& uuid);

        /** Returns an unordered map containing all resources loaded at call time */
        Vector<HResource> GetAll();

        /** Find all resources based on the _coreType (Serializable) */
        Vector<HResource> FindByType(CoreType type);

    public:
        /** Called when a resource has been loaded. Provides a handle to the loaded resource. */
        Event<void(const HResource&)> OnResourceLoaded;

        /** Called when the resource has been destroyed. Provides UUID of the destroyed resource.*/
        Event<void(const UUID&, CoreType type)> OnResourceDestroyed;

        /** Called when the internal resource the handle is pointing to has changed. */
        Event<void(const HResource&)> OnResourceModified;

    private:
        friend class ResourceHandleBase;

        /**	Destroys a resource, freeing its memory. */
        void Destroy(ResourceHandleBase& resource);

        bool GetUUIDFromFile(const std::filesystem::path& filePath, UUID& uuid);
        bool GetFileFromUUID(const UUID& uuid, String& filePath);
        void RegisterResource(const UUID& uuid, const std::filesystem::path& filePath);
        void UnregisterResource(const UUID& uuid);

    private:
        UnorderedMap<UUID, LoadedResourceData> _loadedResources;
        UnorderedMap<UUID, String> _UUIDToFile;
        UnorderedMap<String, UUID> _fileToUUID;

        // In case we use LoadAll, we need to keep a link between primary 
        // resource (which is linked to a file) and all subresources
        UnorderedMap<UUID, Vector<SubResourceUUID>> _resourcesChunks;

        RecursiveMutex _loadingResourceMutex;
        RecursiveMutex _loadingUuidMutex;
    };

    TE_CORE_EXPORT ResourceManager& gResourceManager();
}
