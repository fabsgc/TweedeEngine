#include "TeScriptManager.h"

#include "Utility/TeDynLibManager.h"
#include "Utility/TeDynLib.h"
#include "Utility/TeFileSystem.h"
#include "TeScript.h"
#include "TeNativeScript.h"

namespace te
{
    const String ScriptManager::LIBRARIES_PATH = "Data/Script/";

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

    DynLib* ScriptManager::LoadScriptLibrary(const String& name)
    {
        if (!LibraryExists(name))
        {
            if (!CompileLibrary(name))
                return nullptr;
        }

        DynLib* library = gDynLibManager().Load(name);
        if (library != nullptr)
            _scriptLibraries[name] = library;

        return library;
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
            library = LoadScriptLibrary(name);
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

    bool ScriptManager::CompileLibrary(const String& name)
    {
        return false;
    }

    bool ScriptManager::LibraryExists(const String& name)
    {
        String path = name + "." + DynLib::EXTENSION;
        return FileSystem::Exists(path);
    }

    ScriptManager& gScriptManager()
    {
        return ScriptManager::Instance();
    }
}
