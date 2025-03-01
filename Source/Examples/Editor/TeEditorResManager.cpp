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
    }
}
