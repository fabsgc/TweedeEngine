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

    const String ScriptManager::LIBRARIES_PATH = "Data/Scripts/";

    size_t ScriptManager::HashFunc::operator() (const ScriptIdentifier& key) const
    {
        size_t hash = 0;
        te_hash_combine(hash, key.Name);

        return hash;
    }

    bool ScriptManager::EqualFunc::operator() (const ScriptIdentifier& a, const ScriptIdentifier& b) const
    {
        if (a.Name != b.Name)
            return false;

        return true;
    }

    ScriptManager::ScriptManager()
        : _paused(true)
    { }

    void ScriptManager::OnStartUp()
    {
#ifdef TE_ENGINE_BUILD
        static String appRoot = RAW_APP_ROOT;
#else
        static String rawAppRoot = "";
#endif

        UINT32 folderChanges = 0;
        folderChanges |= (UINT32)FolderChangeFlag::FileName;
        folderChanges |= (UINT32)FolderChangeFlag::FileWrite;

        _folderMonitor.StartMonitor(appRoot + LIBRARIES_PATH, false, folderChanges);

        _folderMonitor.OnAdded.Connect(std::bind(&ScriptManager::OnMonitorFileAdded, this, _1));
        _folderMonitor.OnRemoved.Connect(std::bind(&ScriptManager::OnMonitorFileRemoved, this, _1));
        _folderMonitor.OnModified.Connect(std::bind(&ScriptManager::OnMonitorFileModified, this, _1));
        _folderMonitor.OnRenamed.Connect(std::bind(&ScriptManager::OnMonitorFileRenamed, this, _1, _2));
    }

    void ScriptManager::OnShutDown()
    {
#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
        if (_scripts.size() > 0)
            TE_ASSERT_ERROR(false, "Not all scripts have been unregistered from Script Manager");
#endif

        _folderMonitor.StopMonitorAll();
        _scripts.clear();
        UnloadAll();
    }

    void ScriptManager::PreUpdate()
    {
        bool isRunning = gCoreApplication().GetState().IsFlagSet(ApplicationState::Scripting);
        if (IsPaused() || !isRunning)
            return;

        for (auto& script : _scripts)
        {
            script->PreUpdate();
        }
    }

    void ScriptManager::PostUpdate()
    {
        bool isRunning = gCoreApplication().GetState().IsFlagSet(ApplicationState::Scripting);
        if (IsPaused() || !isRunning)
            return;

        for (auto& script : _scripts)
        {
            script->PostUpdate();
        }
    }

    void ScriptManager::PostRender()
    {
        bool isRunning = gCoreApplication().GetState().IsFlagSet(ApplicationState::Scripting);
        if (IsPaused() || !isRunning)
            return;

        for (auto& script : _scripts)
        {
            script->PostRender();
        }
    }

    void ScriptManager::Update()
    {
        _folderMonitor.Update();
    }

    void ScriptManager::RegisterScript(Script* script)
    {
        if (std::find(_scripts.begin(), _scripts.end(), script) == _scripts.end())
            _scripts.push_back(script);
    }

    void ScriptManager::UnregisterScript(Script* script)
    {
        auto iter = std::find(_scripts.begin(), _scripts.end(), script);

        if (iter != _scripts.end())
            _scripts.erase(iter);
    }

    NativeScript* ScriptManager::CreateNativeScript(const String& name, const String& path)
    {
        return CreateNativeScript(ScriptIdentifier(name, path));
    }

    NativeScript* ScriptManager::CreateNativeScript(const ScriptIdentifier& identifier)
    {
        NativeScript* script = nullptr;
        typedef NativeScript* (*LoadScriptFunc)();
        DynLib* library = GetScriptLibrary(identifier);

        if (library)
        {
            LoadScriptFunc loadScriptFunc = (LoadScriptFunc)library->GetSymbol("LoadScript");

            if (loadScriptFunc)
                script = static_cast<NativeScript*>(loadScriptFunc());
            else
                TE_DEBUG("Script \"" + identifier.Name + "\" has been built but LoadScript() function was not found");
        }

        return script;
    }

    void ScriptManager::DeleteNativeScript(NativeScript* script)
    {
        typedef void (*UnloadScriptFunc)(NativeScript*);
        DynLib* library = GetScriptLibrary(script->GetLibraryName());

        if (library)
        {
            UnloadScriptFunc unloadScriptFunc = (UnloadScriptFunc)library->GetSymbol("UnloadScript");
            unloadScriptFunc(script);
        }
    }

    DynLib* ScriptManager::LoadScriptLibrary(const ScriptIdentifier& identifier)
    {
        if (!LibraryExists(identifier.Name))
        {
            if (!CompileLibrary(identifier))
                return nullptr;
        }

        if (LibraryExists(identifier.Name))
        {
            DynLib* library = gDynLibManager().Load(identifier.Name);
            if (library != nullptr)
            {
                // RecursiveLock lock(_mutex);
                _scriptLibraries[identifier] = library;
            }

            return library;
        }

        return nullptr;
    }

    void ScriptManager::UnloadScriptLibrary(const ScriptIdentifier& identifier, Vector<UnloadedScript>* unloadedScripts)
    {
        // RecursiveLock lock(_mutex);
        auto iter = _scriptLibraries.find(identifier);
        if (iter != _scriptLibraries.end())
        {
            // Remove instances of script which will be unloaded
            for (auto& script : _scripts)
            {
                auto nativeScript = script->GetNativeScript();

                if (nativeScript && identifier.Name == nativeScript->GetLibraryName())
                {
                    if (unloadedScripts)
                    {
                        UnloadedScript unloadedScript;
                        unloadedScript.ScriptToReload = script;
                        unloadedScript.PreviousSceneObject = nativeScript->GetParentSceneObject();

                        (*unloadedScripts).push_back(unloadedScript);
                    }

                    script->OnShutdown();
                    gScriptManager().DeleteNativeScript(script->_nativeScript);
                    script->_nativeScript = nullptr;
                }
            }

            gDynLibManager().Unload(iter->second);
            _scriptLibraries.erase(iter);
        }
    }

    DynLib* ScriptManager::GetScriptLibrary(const ScriptIdentifier& identifier)
    {
        DynLib* library = nullptr;

        {
            // RecursiveLock lock(_mutex);
            auto iter = _scriptLibraries.find(identifier);

            if (iter == _scriptLibraries.end())
                library = LoadScriptLibrary(identifier);
            else
                library = _scriptLibraries[identifier];
        }

        return library;
    }

    void ScriptManager::UnloadAll()
    {
        {
            // RecursiveLock lock(_mutex);
            for (auto& script : _scripts)
            {
                script->OnShutdown();
                gScriptManager().DeleteNativeScript(script->_nativeScript);
                script->_nativeScript = nullptr;
            }
        }

        for (auto it = _scriptLibraries.begin(); it != _scriptLibraries.end();)
        {
            it->second->Unload();
            it = _scriptLibraries.erase(it);
        }
    }

    bool ScriptManager::LibraryExists(const String& name)
    {
        String path = name + "." + DynLib::EXTENSION;
        return FileSystem::Exists(path);
    }

    bool ScriptManager::CheckLastBuildOldEnough(const ScriptIdentifier& identifier)
    {
        if (_lastBuildTimes.find(identifier) == _lastBuildTimes.end())
            return true;

        UINT64 lastBuildTime = _lastBuildTimes[identifier];
        UINT64 now = gTime().GetTimeMs();

        if ((now - lastBuildTime) > 10000)
            return true;

        return false;
    }

    void ScriptManager::OnMonitorFileModified(const String& path)
    {
        std::filesystem::path filePath(path);
        String fileName;
        String fileExtension;
        Vector<UnloadedScript> unloadedScripts;

        if (filePath.has_filename())
        {
            SetPaused(true);
            fileName = filePath.filename().string();
            fileExtension = filePath.extension().string();

            if(fileExtension == ".cpp")
            {
                fileName = ReplaceAll(fileName, fileExtension, "");
                this->UnloadScriptLibrary(fileName, &unloadedScripts);
            }
        }

        auto FileModifiedTask = [this, filePath, fileName, fileExtension, unloadedScripts]() {

            if (filePath.has_filename() && fileExtension == ".cpp")
            {
                if (this->CompileLibrary(fileName))
                {
                    // RecursiveLock lock(_mutex);
                    for (auto& unloadedScript : unloadedScripts)
                    {
                        unloadedScript.ScriptToReload->SetNativeScript(
                            fileName, unloadedScript.PreviousSceneObject, filePath.parent_path().generic_string());
                    }
                }
            }
        };

        auto FileModifiedCallback = [this]() {
            SetPaused(false);
        };

        FileModifiedTask();
        FileModifiedCallback();

        //SPtr<Task> task = Task::Create("OnMonitorFileModified", FileModifiedTask, FileModifiedCallback);
        //gTaskScheduler().AddTask(task); TODO rewrite ScriptManager
    }

    void ScriptManager::OnMonitorFileAdded(const String& path)
    {
        // NOTHING
    }

    void ScriptManager::OnMonitorFileRemoved(const String& path)
    {
        std::filesystem::path filePath(path);
        if (filePath.has_filename())
        {
            String fileName = filePath.filename().string();
            String fileExtension = filePath.extension().string();

            if (fileExtension == ".cpp")
            {
                fileName = ReplaceAll(fileName, fileExtension, "");
                UnloadScriptLibrary(fileName);
            }
        }
    }

    void ScriptManager::OnMonitorFileRenamed(const String& from, const String& to)
    {
        auto FileRenamedTask = [this, from, to]() {
            // RecursiveLock lock(_mutex);
            std::filesystem::path oldFilePath(from);
            std::filesystem::path newFilePath(to);

            SetPaused(true);

            if (oldFilePath.has_filename() && newFilePath.has_filename())
            {
                String oldFileName = oldFilePath.filename().string();
                String newFileName = newFilePath.filename().string();
                String oldFileExtension = oldFilePath.extension().string();
                String newFileExtension = newFilePath.extension().string();

                if (oldFileExtension == ".cpp")
                {
                    oldFileName = ReplaceAll(oldFileName, oldFileExtension, "");

                    Vector<UnloadedScript> unloadedScripts;
                    UnloadScriptLibrary(oldFileName, &unloadedScripts);
                    if (unloadedScripts.size() > 0 && CompileLibrary(newFileName))
                    {
                        if (newFileExtension == ".cpp")
                        {
                            newFileName = ReplaceAll(newFileName, newFileExtension, "");
                            for (auto& unloadedScript : unloadedScripts)
                            {
                                unloadedScript.ScriptToReload->SetNativeScript(
                                    newFileName, unloadedScript.PreviousSceneObject, newFilePath.parent_path().generic_string());
                            }
                        }
                    }
                }
            }

            SetPaused(false);
        };

        FileRenamedTask();

        //SPtr<Task> task = Task::Create("OnMonitorFileRenamed", FileRenamedTask);
        //gTaskScheduler().AddTask(task); TODO rewrite ScriptManager
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
