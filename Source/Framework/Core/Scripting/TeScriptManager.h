#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"
#include "Utility/TeTime.h"
#include "Platform/TeFolderMonitor.h"
#include "Threading/TeThreading.h"

#include <filesystem>

namespace te
{
    class NativeScript;

    struct UnloadedScript
    {
        Script* ScriptToReload;
        HSceneObject PreviousSceneObject;
    };

    struct ScriptIdentifier
    {
        String Name; // Each script name loaded must be unique
        String AbsolutePath;

        bool operator <(const ScriptIdentifier& rhs) const
        {
            return Name < rhs.Name;
        }

        ScriptIdentifier(const String& name, const String& path = "")
            : Name(name)
            , AbsolutePath(path)
        {
            if (!path.empty())
            {
                std::filesystem::path absoluteTempPath(AbsolutePath);
                std::filesystem::path tempPath = std::filesystem::absolute(absoluteTempPath);
                if (std::filesystem::is_directory(tempPath) == false)
                    tempPath = tempPath.parent_path();

#if TE_PLATFORM == TE_PLATFORM_WIN32
                AbsolutePath = std::filesystem::absolute(tempPath).generic_string() + "\\";
#else
                AbsolutePath = std::filesystem::absolute(tempPath).generic_string() + "/";
#endif
            }
            else
            {
#if TE_PLATFORM == TE_PLATFORM_WIN32
                static String appRoot = ReplaceAll(RAW_APP_ROOT, "/", "\\");
                static String librariesPath = ReplaceAll("Data/Scripts/", "/", "\\");
                AbsolutePath = appRoot + librariesPath;
#else
                static String librariesPath = "Data/Scripts/";
                AbsolutePath = RAW_APP_ROOT + librariesPath;
#endif
            }
        }

        friend bool operator==(const ScriptIdentifier& lhs, const ScriptIdentifier& rhs)
        {
            return lhs.Name == rhs.Name;
        }
    };

    /**	Handles initialization of a scripting system. */
    class TE_CORE_EXPORT ScriptManager : public Module<ScriptManager>
    {
    public:
        ScriptManager();
        ~ScriptManager() = default;

        TE_MODULE_STATIC_HEADER_MEMBER(ScriptManager)

        void OnStartUp() override;
        void OnShutDown() override;

        /** When a Script (so a CScript) is created, we need to register this script inside the manager */
        void RegisterScript(Script* script);

        /** When a Script is destroyed, we want to remove this script from the manager */
        void UnregisterScript(Script* script);

        /** When a script need a new instance of the given native script */
        NativeScript* CreateNativeScript(const String& name, const String& path = "");

        /** @copydoc ScriptManager::CreateNativeScript */
        NativeScript* CreateNativeScript(const ScriptIdentifier& identifier);

        /** Memory management for NativeScript, is manual, each script has an exported function for deleting a NativeScript */
        void DeleteNativeScript(NativeScript* script);

        /** Unloads all script libraries loaded in the engine */
        void UnloadAll();

        /** Returns all currently loaded script libraries */
        const Map<ScriptIdentifier, DynLib*>& GetScriptLibraries() const { return _scriptLibraries; }

        /** Returns all currently instanciated scripts */
        const Vector<Script*>& GetScripts() const { return _scripts; }

        void TogglePaused();

        void SetPaused(bool paused);

        bool IsPaused() const { return _paused; };

    public: // #### EVENTS FOR SCRIPTS
        /** Called once per frame before scene update. */
        void PreUpdate();

        /** Called once per frame after scene update */
        void PostUpdate();

        /** Called once per frame after engine render */
        void PostRender();

        /** Update any script which has been modified */
        void Update();

    public: // #### EVENTS FOR SCRIPTS FOLRDER WATCHING
        /**	Triggered when the native folder monitor detects a file has been modified. */
        void OnMonitorFileModified(const String& path);

        /**	Triggered when the native folder monitor detects a file has been added. */
        void OnMonitorFileAdded(const String& path);

        /**	Triggered when the native folder monitor detects a file has been removed. */
        void OnMonitorFileRemoved(const String& path);

        /**	Triggered when the native folder monitor detects a file has been renamed. */
        void OnMonitorFileRenamed(const String& from, const String& to);

    private:
        /** Try to load a new script lib (.dll, .so) */
        DynLib* LoadScriptLibrary(const ScriptIdentifier& identifier);

        /** 
         * Try to unload a new script lib (.dll, .so), 
         * if second argument is not null, it's filled with pointer to scripts which are using this nativeScript 
         */
        void UnloadScriptLibrary(const ScriptIdentifier& identifier, Vector<UnloadedScript>* unloadedScripts = nullptr);

        /** Returns (and loads if not loaded yet) the given dynamic library */
        DynLib* GetScriptLibrary(const ScriptIdentifier& identifier);

        /** Compiles a library using provided name. All libraries will be located in the same directory as dlls and binaries */
        bool CompileLibrary(const ScriptIdentifier& identifier);

        /** Check if a library already exists. Usefull if we don't want to compile everything (time consuming) */
        bool LibraryExists(const String& name);

        /** On Win32, file monitoring triggers twice when file modified (so two build) */
        bool CheckLastBuildOldEnough(const ScriptIdentifier& identifier);

    public:
        /** Path where the debug configuration managed assemblies are located at, relative to the working directory. */
        static const String LIBRARIES_PATH;

    private:
        Map<ScriptIdentifier, DynLib*> _scriptLibraries;
        Map<ScriptIdentifier, UINT64> _lastBuildTimes;
        Vector<Script*> _scripts;
        FolderMonitor _folderMonitor;
        RecursiveMutex _mutex;
        bool _paused;
    };

    /** Provides easy access to the ScriptManager. */
    TE_CORE_EXPORT ScriptManager& gScriptManager();
}