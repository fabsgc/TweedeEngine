#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"
#include "Resources/TeResourceManager.h"

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
                    return it->second.GetNewHandleFromExisting();

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
                return static_resource_cast<T>(gResourceManager().Get(resource.GetUUID()));
            }

            return static_resource_cast<T>(resource);
        }

        template <class T>
        void Add(ResourceHandle<T>& handle)
        {
            HResource resource = handle.GetNewHandleFromExisting();
            _resources[T::GetResourceType()].Add(resource);
        }

        template <class T>
        void Remove(ResourceHandle<T>& handle)
        {
            HResource resource = handle.GetNewHandleFromExisting();
            _resources[T::GetResourceType()].Remove(resource);
        }

        template <class T>
        ResourcesContainer& Get()
        {
            return _resources[T::GetResourceType()];
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
    };
}
