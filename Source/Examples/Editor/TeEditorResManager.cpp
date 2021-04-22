#include "TeEditorResManager.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(EditorResManager)

    EditorResManager::EditorResManager()
    { }

    EditorResManager::~EditorResManager()
    { }

    SPtr<MultiResource> EditorResManager::LoadAll(const String& filePath, const SPtr<const ImportOptions>& options, bool force)
    {
        SPtr<MultiResource> resources = gResourceManager().LoadAll(filePath, options, force);
        Vector<SubResource> output;

        for(auto& subRes : resources->Entries)
        {
            if (subRes.Res.IsLoaded())
            {
                _resources[subRes.Res->GetCoreType()].Add(subRes.Res);
                output.push_back(subRes);
            }
        }

        return te_shared_ptr_new<MultiResource>(output);
    }

    void EditorResManager::OnStartUp()
    { }

    void EditorResManager::OnShutDown()
    { 
        _resources.clear();
    }
}
