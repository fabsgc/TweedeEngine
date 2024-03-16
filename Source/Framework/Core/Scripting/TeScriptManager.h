#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"
#include "Utility/TeTime.h"
#include "Platform/TeFolderMonitor.h"
#include "Threading/TeThreading.h"

#include <filesystem>

namespace te
{
    /**	Handles initialization of a scripting system. */
    class TE_CORE_EXPORT ScriptManager : public Module<ScriptManager>
    {
    public:
        ScriptManager();
        virtual ~ScriptManager() = default;

        TE_MODULE_STATIC_HEADER_MEMBER(ScriptManager)

        void OnStartUp() override;
        void OnShutDown() override;

        /** Register a Script attached to the given SceneObject and creates the associated NativeScript */
        bool RegisterScript(const HScript& script, SceneObject& so);

        /** Unregister a script */
        void UnregisterScript(const HScript& script, SceneObject& so);

        /** When a script need a new instance of the given native script */
        NativeScript* CreateNativeScript(const HScript& script, SceneObject& so);

        /** Memory management for NativeScript, is manual, each script has an exported function for deleting a NativeScript */
        void DeleteNativeScript(const Script* script, NativeScript* nativeScript);

        /** Unloads all script libraries loaded in the engine */
        void UnloadAll();

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

    private:
        friend class Script;

        /** Try to load a new script lib (.dll, .so) */
        DynLib* LoadScriptLibrary(const Script* script);

        /**
         * Try to unload a script lib (.dll, .so),
         * if second argument is not null, it's filled with pointer to scripts which are using this nativeScript
         */
        void UnloadScriptLibrary(const Script* script);

        /** Returns (and loads if not loaded yet) the given dynamic library */
        DynLib* GetScriptLibrary(const Script* script);

        /** Compiles a library using provided name. All libraries will be located in the same directory as dlls and binaries */
        bool CompileLibrary(const Script* script);

        /** Check if a library already exists. Usefull if we don't want to compile everything (time consuming) */
        bool LibraryExists(const Script* script);

        /** On Win32, file monitoring triggers twice when file modified (so two build) */
        bool CheckLastBuildOldEnough(const Script* script);

    private:
        Map<const Script*, DynLib*> _scriptLibraries;
        Map<const Script*, UINT64> _lastBuildTimes;
        Map<Pair<const Script*, SceneObject*>, NativeScript*> _runningScripts;

        bool _paused;
    };

    /** Provides easy access to the ScriptManager. */
    TE_CORE_EXPORT ScriptManager& gScriptManager();
}
