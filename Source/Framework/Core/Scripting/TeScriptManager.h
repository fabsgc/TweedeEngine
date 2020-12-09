#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"

namespace te
{
    class NativeScript;

    /**	Handles initialization of a scripting system. */
    class TE_CORE_EXPORT ScriptManager : public Module<ScriptManager>
    {
    public:
        ScriptManager() = default;
        ~ScriptManager() = default;

        void OnShutDown() override;

        /** When a Script (so a CScript) is created, we need to register this script inside the manager */
        void RegisterScript(Script* script);

        /** When a Script is destroyed, we want to remove this script from the manager */
        void UnregisterScript(Script* script);

        /** When a script need a new instance of the given native script */
        NativeScript* CreateNativeScript(const String& name);

        /** Unloads all script libraries loaded in the engine */
        void UnloadAll();

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
        /** Try to load a new script lib (.dll, .so) */
        void LoadScriptLibrary(const String& name, DynLib** library);

        /** Try to unload a new script lib (.dll, .so) */
        void UnloadScriptLibrary(const String& name);

        /** Returns (and loads if not loaded yet) the given dynamic library */
        DynLib* GetScriptLibrary(const String& name);

    private:
        UnorderedMap<String, DynLib*> _scriptLibraries;
        Vector<Script*> _scripts;
    };

    /** Provides easy access to the ScriptManager. */
    TE_CORE_EXPORT ScriptManager& gScriptManager();
}