#include "Scripting/TeScriptManager.h"

#include "Scripting/TeNativeScript.h"
#include "Scripting/TeScript.h"
#include "Utility/TeDynLibManager.h"
#include "Utility/TeDynLib.h"

#include "TeCoreApplication.h"

#include <filesystem>

using namespace std::placeholders;

namespace te
{
    typedef void (*UnloadScriptFunc)(NativeScript*);

    TE_MODULE_STATIC_MEMBER(ScriptManager)

    ScriptManager::ScriptManager()
        : _paused(true)
    { }

    void ScriptManager::OnStartUp()
    { }

    void ScriptManager::OnShutDown()
    {
#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
        if (_runningScripts.size() > 0)
            TE_ASSERT_ERROR(false, "Not all scripts have been unregistered from Script Manager");
#endif

        UnloadAll();
    }

    bool ScriptManager::RegisterScript(const HScript& script, SceneObject& so)
    {
        if (script.IsLoaded())
        {
            Pair<Script*, SceneObject*> identifier = std::make_pair(script.GetInternalPtr().get(), &so);

            if (_runningScripts.find(identifier) == _runningScripts.end())
            {
                NativeScript* nativeScript = CreateNativeScript(script, so);

                if (nativeScript)
                {
                    _runningScripts[identifier] = nativeScript;
                }
                else
                {
                    _runningScripts[identifier] = nullptr;
                }

                return true;
            }
        }

        return false;
    }

    void ScriptManager::UnregisterScript(const HScript& script, SceneObject& so)
    {
        const Pair<const Script*, SceneObject*> identifier = std::make_pair(script.GetInternalPtr().get(), &so);

        const auto runningScriptIt = _runningScripts.find(identifier);
        if (runningScriptIt != _runningScripts.end())
        {
            DeleteNativeScript(runningScriptIt->first.first, runningScriptIt->second);
            _runningScripts.erase(runningScriptIt);
        }
    }

    NativeScript* ScriptManager::CreateNativeScript(const HScript& script, SceneObject& so)
    {
        NativeScript* nativeScript = nullptr;
        typedef NativeScript* (*LoadScriptFunc)();
        DynLib* library = GetScriptLibrary(script.GetInternalPtr().get());

        if (library)
        {
            LoadScriptFunc loadScriptFunc = (LoadScriptFunc)library->GetSymbol("LoadScript");

            if (loadScriptFunc)
            {
                nativeScript = static_cast<NativeScript*>(loadScriptFunc());
                nativeScript->SetParentSceneObject(so);
                nativeScript->OnStartup();
            }
            else
            {
                TE_DEBUG("Script \"" + script->GetName() + "\" has been built but LoadScript() function was not found");
            }
        }

        return nativeScript;
    }

    void ScriptManager::DeleteNativeScript(const Script* script, NativeScript* nativeScript)
    {
        DynLib* library = GetScriptLibrary(script);
        if (nativeScript)
        {
            nativeScript->OnShutdown();
        }

        if (library)
        {
            UnloadScriptFunc unloadScriptFunc = (UnloadScriptFunc)library->GetSymbol("UnloadScript");
            unloadScriptFunc(nativeScript);
        }
    }

    void ScriptManager::UnloadAll()
    {
        for (auto& runningScript : _runningScripts)
        {
            DeleteNativeScript(runningScript.first.first, runningScript.second);
        }

        for (auto& scriptLibrary : _scriptLibraries)
        {
            scriptLibrary.second->Unload();
        }

        _runningScripts.clear();
        _scriptLibraries.clear();
    }

    DynLib* ScriptManager::LoadScriptLibrary(const Script* script)
    {
        if (!LibraryExists(script))
        {
            if (!CompileLibrary(script))
                return nullptr;
        }

        if (LibraryExists(script))
        {
            DynLib* library = gDynLibManager().Load(script->GetUUID().ToString());
            if (library != nullptr)
            {
                _scriptLibraries[script] = library;
            }

            return library;
        }

        return nullptr;
    }

    void ScriptManager::UnloadScriptLibrary(const Script* script)
    {
        auto iter = _scriptLibraries.find(script);
        if (iter != _scriptLibraries.end())
        {
            // Remove all running scripts attached to this Script
            for (auto& runningScript : _runningScripts)
            {
                if (runningScript.first.first)
                {
                    DeleteNativeScript(runningScript.first.first, runningScript.second);
                    _runningScripts.erase(std::make_pair(script, runningScript.first.second));
                }
            }

            gDynLibManager().Unload(iter->second);
            _scriptLibraries.erase(iter);
        }
    }

    DynLib* ScriptManager::GetScriptLibrary(const Script* script)
    {
        DynLib* library = (_scriptLibraries.find(script) == _scriptLibraries.end()) ? LoadScriptLibrary(script) : _scriptLibraries[script];
        return library;
    }

    bool ScriptManager::LibraryExists(const Script* script)
    {
        return std::filesystem::exists(script->GetUUID().ToString() + "." + DynLib::EXTENSION);
    }

    bool ScriptManager::CheckLastBuildOldEnough(const Script* script)
    {
        if (_lastBuildTimes.find(const_cast<Script*>(script)) == _lastBuildTimes.end())
            return true;

        UINT64 lastBuildTime = _lastBuildTimes[script];
        UINT64 now = gTime().GetTimeMs();

        if ((now - lastBuildTime) > 10000)
            return true;

        return false;
    }

    void ScriptManager::PreUpdate()
    {
        bool isRunning = gCoreApplication().GetState().IsFlagSet(ApplicationState::Scripting);
        if (IsPaused() || !isRunning)
            return;

        for (auto& script : _runningScripts)
        {
            script.second->PreUpdate();
        }
    }

    void ScriptManager::PostUpdate()
    {
        bool isRunning = gCoreApplication().GetState().IsFlagSet(ApplicationState::Scripting);
        if (IsPaused() || !isRunning)
            return;

        for (auto& script : _runningScripts)
        {
            script.second->PostUpdate();
        }
    }

    void ScriptManager::PostRender()
    {
        bool isRunning = gCoreApplication().GetState().IsFlagSet(ApplicationState::Scripting);
        if (IsPaused() || !isRunning)
            return;

        for (auto& script : _runningScripts)
        {
            script.second->PostRender();
        }
    }

    void ScriptManager::Update()
    {
        bool isRunning = gCoreApplication().GetState().IsFlagSet(ApplicationState::Scripting);
        if (IsPaused() || !isRunning)
            return;

        for (auto& script : _runningScripts)
        {
            script.second->Update();
        }
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
