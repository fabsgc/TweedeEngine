#include "TeEditorResManager.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(EditorResManager)

    EditorResManager::EditorResManager()
    { }

    EditorResManager::~EditorResManager()
    { }

    void EditorResManager::OnStartUp()
    { }

    void EditorResManager::OnShutDown()
    { 
        _resources.clear();
    }
}
