#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"
#include "Resources/TeResourceManager.h"
#include "Resources/TeResource.h"

namespace te
{
    class EditorResManager : public Module<EditorResManager>
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

            void Remove(HResource& resource)
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

        EditorResManager();
        virtual ~EditorResManager();

        /** @copydoc Module::OnStartUp */
        void OnStartUp() override;

        /** @copydoc Module::OnShutDown */
        void OnShutDown() override;

        /** copydoc ResourceManager::LoadAll */
        SPtr<MultiResource> LoadAll(const String& filePath, const SPtr<const ImportOptions>& options = nullptr, bool force = false);

        template <class T>
        ResourceHandle<T> Load(const String& filePath, const SPtr<const ImportOptions>& options, bool force = false)
        {
            HResource resource = gResourceManager().Load<T>(filePath, options, force);

            if (resource.IsLoaded())
            {
                _resources[T::GetResourceType()].Add(resource);
                _resourcesIndex.push_back(resource.GetInternalPtr().get());
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
                _resourcesIndex.push_back(handle.GetInternalPtr().get());
        }

        template <class T>
        void Remove(ResourceHandle<T>& handle)
        {
            HResource r = static_resource_cast<Resource>(handle);
            _resources[T::GetResourceType()].Remove(r);

            if (handle.IsLoaded())
            {
                auto it = std::find(_resourcesIndex.begin(), _resourcesIndex.end(), handle.GetInternalPtr().get());
                if (it != _resourcesIndex.end())
                    _resourcesIndex.erase(it);
            }
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
        UnorderedMap<UINT32, ResourcesContainer> _resources;
        Vector<Resource*> _resourcesIndex;
    };
}
