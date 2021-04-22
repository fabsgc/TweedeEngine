#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"
#include "Resources/TeResourceManager.h"
#include <vector>
#include <unordered_map>

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

            std::unordered_map<UUID, HResource> Res;
        };

    public:
        TE_MODULE_STATIC_HEADER_MEMBER(EditorResManager)

        EditorResManager();
        ~EditorResManager();

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

    protected:
        std::unordered_map<UINT32, ResourcesContainer> _resources;
    };
}
