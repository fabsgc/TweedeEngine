#include "TeEditorResManager.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(EditorResManager)

    SPtr<MultiResource> EditorResManager::LoadAll(const std::filesystem::path& filePath, const ImportOptions& options, ResourceManager::LoadingMode mode)
    {
        SPtr<MultiResource> resources = gResourceManager().LoadAll(filePath, options, mode);
        Vector<SubResource> output;

        for(auto& subRes : resources->Entries)
        {
            if (subRes.Res.IsLoaded())
            {
                Remove(_resources[subRes.Res->GetCoreType()].Find(subRes.Res->GetUUID()));

                _resources[subRes.Res->GetCoreType()].Add(subRes.Res);
                _resourcesIndex.push_back(subRes.Res.Get());

                output.push_back(subRes);                    
            }
        }

        return te_shared_ptr_new<MultiResource>(output);
    }

    void EditorResManager::OnShutDown()
    { 
        _resources.clear();

        // For modules, we need to call Disconnect on event during shutdown
        _onResourceModified.Disconnect();
        _onResourceDestroyed.Disconnect();
    }

    void EditorResManager::OnResourceModified(const HResource& resource)
    {
        if (resource.IsLoaded())
        {
            CoreType type = resource->GetCoreType();
            auto it = _resources[type].Find(resource.GetUUID());

            if (it.IsLoaded())
            {
                _resources[type].Remove(it);

                _resources[type].Add(const_cast<HResource&>(resource));
                _resourcesIndex.push_back(resource.Get());
            }
        }
    }

    void EditorResManager::OnResourceDestroyed(const UUID& uuid, CoreType type)
    {
        Remove(_resources[type].Find(uuid));
    }
}
