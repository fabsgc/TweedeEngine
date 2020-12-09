#include "TeScriptManager.h"

#include "Utility/TeDynLibManager.h"
#include "Utility/TeDynLib.h"
#include "TeScript.h"

namespace te
{
    void ScriptManager::OnShutDown()
    {
#if TE_DEBUG_MODE
        if (_scripts.size() > 0)
            TE_ASSERT_ERROR(false, "Not all scripts have been unregistered from Script Manager");
#endif

        _scripts.clear();
        UnloadAll();
    }

    void ScriptManager::PreUpdate()
    { 
        for (auto& script : _scripts)
        {
            script->PreUpdate();
        }
    }

    void ScriptManager::PostUpdate()
    { 
        for (auto& script : _scripts)
        {
            script->PostUpdate();
        }
    }

    void ScriptManager::PostRender()
    { 
        for (auto& script : _scripts)
        {
            script->PostRender();
        }
    }

    void ScriptManager::Update()
    { 
        for (auto& script : _scripts)
        {
            script->Update();
        }
    }

    void ScriptManager::RegisterScript(Script* script)
    {
        if (std::find(_scripts.begin(), _scripts.end(), script) == _scripts.end())
            _scripts.push_back(script);
    }

    void ScriptManager::UnregisterScript(Script* script)
    {
        auto iter = std::find(_scripts.begin(), _scripts.end(), script);

        if(iter != _scripts.end())
            _scripts.erase(iter);
    }

    NativeScript* ScriptManager::CreateNativeScript(const String& name)
    {
        NativeScript* script = nullptr;
        typedef NativeScript* (*LoadScriptFunc)();
        DynLib* library = GetScriptLibrary(name);

        if (library)
        {
            LoadScriptFunc loadScriptFunc = (LoadScriptFunc)library->GetSymbol("LoadScript");
            script = loadScriptFunc();
        }

        return script;
    }

    void ScriptManager::LoadScriptLibrary(const String& name, DynLib** library)
    {
        DynLib* loadedLibrary = gDynLibManager().Load(name);
        if (library != nullptr)
        {
            *library = loadedLibrary;
            _scriptLibraries[name] = loadedLibrary;
        }
    }

    void ScriptManager::UnloadScriptLibrary(const String& name)
    {
        auto iter = _scriptLibraries.find(name);
        if (iter != _scriptLibraries.end())
        {
            iter->second->Unload();
            _scriptLibraries.erase(iter);
        }
    }

    DynLib* ScriptManager::GetScriptLibrary(const String& name)
    {
        DynLib* library = nullptr;
        auto iter = _scriptLibraries.find(name);

        if (iter == _scriptLibraries.end())
        {
            LoadScriptLibrary(name, &library);
        }
        else
        {
            library = _scriptLibraries[name];
        }

        return library;
    }

    void ScriptManager::UnloadAll()
    {
        for (auto it = _scriptLibraries.begin(); it != _scriptLibraries.end();)
        {
            it->second->Unload();
            it = _scriptLibraries.erase(it);
        }
    }

    ScriptManager& gScriptManager()
    {
        return ScriptManager::Instance();
    }
}
