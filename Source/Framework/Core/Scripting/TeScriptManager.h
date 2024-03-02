#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"
#include "Utility/TeTime.h"
#include "Platform/TeFolderMonitor.h"
#include "Threading/TeThreading.h"

#include <filesystem>

namespace te
{
    struct ScriptIdentifier
    {
        String Name;
        String AbsolutePath;
    };

    /**	Handles initialization of a scripting system. */
    class TE_CORE_EXPORT ScriptManager : public Module<ScriptManager>
    {
    public:
        ScriptManager();
        virtual ~ScriptManager() = default;

        TE_MODULE_STATIC_HEADER_MEMBER(ScriptManager)

        void OnStartUp() override;
        void OnShutDown() override;

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
        /** Compiles a library using provided name. */
        bool CompileLibrary();

    private:
        bool _paused;
    };

    /** Provides easy access to the ScriptManager. */
    TE_CORE_EXPORT ScriptManager& gScriptManager();
}
