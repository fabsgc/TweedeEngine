#include "TeScriptManager.h"

namespace te
{
    ScriptManager::ScriptManager()
    { }

    ScriptManager::~ScriptManager()
    { }

    void ScriptManager::PreUpdate()
    { }

    void ScriptManager::PostUpdate()
    { }

    void ScriptManager::PostRender()
    { }

    void ScriptManager::Update()
    { }

    ScriptManager& gScriptManager()
    {
        return ScriptManager::Instance();
    }
}
