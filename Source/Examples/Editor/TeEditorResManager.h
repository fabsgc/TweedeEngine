#pragma once

#include "TeCorePrerequisites.h"

#include "Utility/TeModule.h"
#include "Resources/TeResourceManager.h"
#include "Resources/TeResourceListener.h"
#include "Resources/TeResource.h"

namespace te
{
    class EditorResManager : public Module<EditorResManager>, public ResourceListener
    {
    public:
        TE_MODULE_STATIC_HEADER_MEMBER(EditorResManager)

        struct ResourcesContainer
        {
            ~ResourcesContainer()
            {
                Res.clear();
            }

            const HResource& operator[](const UUID& uuid)
            {
                if (Res.count(uuid))
                    return Res[uuid];

                assert(false);
                return Res[UUID::EMPTY];
            }

            void Add(HResource& resource)
            {
                Res[resource.GetUUID()] = resource;
            }

            void Remove(const HResource& resource)
            {
                auto it = Res.find(resource.GetUUID());
                if (it != Res.end())
                    Res.erase(it);
            }

            HResource Find(const UUID& uuid)
            {
                auto it = Res.find(uuid);
                if (it != Res.end())
                    return it->second;

                return HResource();
            }

            UnorderedMap<UUID, HResource> Res;
        };

    public:
        TE_MODULE_STATIC_HEADER_MEMBER(EditorResManager)

        EditorResManager() = default;

        virtual ~EditorResManager() = default;

        /** @copydoc Module::OnStartUp */
        void OnStartUp() override {}

        /** @copydoc Module::OnShutDown */
        void OnShutDown() override;

        /** copydoc ResourceManager::LoadAll */
        SPtr<MultiResource> LoadAll(const std::filesystem::path& filePath, const ImportOptions& options, ResourceManager::LoadingMode loadingMode = ResourceManager::LoadingMode::KeepExisting);

        template <class T>
        ResourceHandle<T> Load(const std::filesystem::path& filePath, const ImportOptions& options, ResourceManager::LoadingMode loadingMode = ResourceManager::LoadingMode::KeepExisting)
        {
            HResource resource = gResourceManager().Load<T>(filePath, options, loadingMode);

            if (resource.IsLoaded())
            {
                _resources[T::GetResourceType()].Add(resource);
                _resourcesIndex.push_back(resource.Get());
                return static_resource_cast<T>(gResourceManager().Get(resource.GetUUID()));
            }

            return static_resource_cast<T>(resource);
        }

        template <class T>
        void Add(ResourceHandle<T>& handle)
        {
            HResource r = static_resource_cast<Resource>(handle);
            _resources[T::GetResourceType()].Add(r);

            if (handle.IsLoaded())
                _resourcesIndex.push_back(handle.Get());
        }

        void Add(HResource& handle)
        {
            _resources[handle->GetCoreType()].Add(handle);

            if (handle.IsLoaded())
                _resourcesIndex.push_back(handle.Get());
        }

        template <class T>
        void Remove(const ResourceHandle<T>& handle)
        {
            if (!handle.IsLoaded())
                return;

            HResource r = static_resource_cast<Resource>(handle);
            _resources[handle->GetCoreType()].Remove(r);

            auto it = std::find(_resourcesIndex.begin(), _resourcesIndex.end(), handle.Get());
            if (it != _resourcesIndex.end())
                _resourcesIndex.erase(it);
        }

        template <class T>
        ResourcesContainer& Get()
        {
            return _resources[T::GetResourceType()];
        }

        Vector<Resource*>& GetAllResources() 
        {
            return _resourcesIndex; 
        }

        template <class T>
        ResourceHandle<T> Find(const UUID& uuid)
        {
            HResource res = _resources[T::GetResourceType()].Find(uuid);
            if (res.IsLoaded())
                return static_resource_cast<T>(res);

            return ResourceHandle<T>();
        }

        void Clear()
        {
            _resources.clear();
            _resourcesIndex.clear();
        }

        void RemoveAndClear()
        {
            for (auto& container : _resources)
            {
                for (auto& resource : container.second.Res)
                {
                    gResourceManager().Release(resource.second);
                }
            }

            Clear();
        }

    protected:
        /** @copydoc ResourceListener::OnResourceModified */
        void OnResourceModified(const HResource& resource) override {}

        /** @copydoc ResourceListener::OnResourceDestroyed */
        void OnResourceDestroyed(const UUID& uuid, CoreType type) override;

    protected:
        UnorderedMap<CoreType, ResourcesContainer> _resources;
        Vector<Resource*> _resourcesIndex;
    };
}
