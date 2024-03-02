#include "TeScriptManager.h"

#include "Utility/TeDynLibManager.h"
#include "Utility/TeDynLib.h"
#include "Utility/TeFileSystem.h"
#include "TeScript.h"
#include "TeNativeScript.h"
#include "TeCoreApplication.h"

#include <filesystem>

using namespace std::placeholders;

namespace te
{
    TE_MODULE_STATIC_MEMBER(ScriptManager)

    ScriptManager::ScriptManager()
        : _paused(true)
    { }

    void ScriptManager::OnStartUp()
    { }

    void ScriptManager::OnShutDown()
    { }

    void ScriptManager::PreUpdate()
    {
        bool isRunning = gCoreApplication().GetState().IsFlagSet(ApplicationState::Scripting);
        if (IsPaused() || !isRunning)
            return;
    }

    void ScriptManager::PostUpdate()
    {
        bool isRunning = gCoreApplication().GetState().IsFlagSet(ApplicationState::Scripting);
        if (IsPaused() || !isRunning)
            return;
    }

    void ScriptManager::PostRender()
    {
        bool isRunning = gCoreApplication().GetState().IsFlagSet(ApplicationState::Scripting);
        if (IsPaused() || !isRunning)
            return;
    }

    void ScriptManager::Update()
    {

    }

    void ScriptManager::SetPaused(bool paused)
    {
        _paused = paused;
    }

    void ScriptManager::TogglePaused()
    {
        _paused = !_paused;
    }

    ScriptManager& gScriptManager()
    {
        return ScriptManager::Instance();
    }
}
