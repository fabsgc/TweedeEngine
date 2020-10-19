#pragma once

#include "TeCorePrerequisites.h"
#include "RenderAPI/TeRenderWindow.h"
#include "Utility/TeModule.h"

namespace te
{
    /**	Structure containing parameters for starting the application. */
    struct START_UP_DESC
    {
        String RenderAPI; /** Name of the render system plugin to use. */
        String Renderer; /** Name of the renderer plugin to use. */
        String Physics; /** Name of the physics plugin to use. */
        String Audio; /** Name of the audio plugin to use. */
        String Gui; /** Name of the gui plugin to use. */

        RENDER_WINDOW_DESC WindowDesc; /** Describes the window to create during start-up. */

        Vector<String> Importers; /** A list of importer plugins to load. */
    };

    /** Represents the current state of the application */
    class ApplicationState
    {
    public:
        enum Mode : UINT32
        {
            Physics = 1UL << 0, // Should the physics tick
            Game = 1UL << 1,    // Is the engine running in game or editor mode?
        };

        ApplicationState()
            : _flags(0)
        {
            _flags |= Mode::Physics;
            _flags |= Mode::Game;
        }

        /** Enables or disabled a flag controlling application state. */
        void SetFlag(Mode flag, bool enabled) { 
            if (enabled) _flags |= flag; 
            else _flags &= ~flag; 
        }

        /** Toggle a flag controlling application state. */
        void ToggleFlag(Mode flag) { 
            _flags = !IsFlagSet(flag) ? _flags | flag : _flags & ~flag; 
        }

        /** Checks if the application has a certain flag enabled. */
        bool IsFlagSet(Mode flag) const { 
            return _flags & flag;
        }

    protected:
        UINT32 _flags;
    };

    /**
     * Represents the primary entry point for the core systems. Handles start-up, shutdown, primary loop and allows you to
     * load and unload plugins.
     */
    class TE_CORE_EXPORT CoreApplication : public Module<CoreApplication>
    {
    public:
        CoreApplication(START_UP_DESC desc);
        virtual ~CoreApplication();

        TE_MODULE_STATIC_HEADER_MEMBER(CoreApplication)

        /**
         * Executes the main loop. This will update your components and modules, queue objects for rendering and run
         * the simulation. Usually called immediately after startUp().
         *
         * This will run infinitely until stopMainLoop is called (usually from another thread or internally).
         */
        void RunMainLoop();

        /**	Stops the (infinite) main loop from running. The loop will complete its current cycle before stopping. */
        void StopMainLoop();

        /**	Pause (or not) the infinite loop */
        void Pause(bool pause);

        /**	Return if the application is on pause or not */
        bool GetPaused();

        ApplicationState& GetState() { return _state; }

        /** Changes the maximum FPS the application is allowed to run in. Zero means unlimited. */
        void SetFPSLimit(UINT32 limit);

        /** Check if the number of FPS has already been reached. */
        void CheckFPSLimit();

        /** Display frame rate on window titlebar */
        void DisplayFrameRate();

        /** Issues a request for the application to close. Application may choose to ignore the request */
        virtual void OnStopRequested();

        /** Issues a request for the application to pause. Application may choose to ignore the request */
        virtual void OnPauseRequested();

        /**	Returns the main window that was created on application start-up. */
        SPtr<RenderWindow> GetWindow() const { return _window; }

        /**	Returns startup desc. */
        const START_UP_DESC& GetStartUpDesc() const { return _startUpDesc; }

        /**
         * Loads a plugin.
         *
         * @param[in]	pluginName	Name of the plugin to load, without extension.
         * @param[out]	library		Specify as not null to receive a reference to the loaded library.
         * @param[in]	passThrough	Optional parameter that will be passed to the loadPlugin function.
         * @return					Value returned from the plugin start-up method.
         */
        void* LoadPlugin(const String& pluginName, DynLib** library = nullptr, void* passThrough = nullptr);

        /**	Unloads a previously loaded plugin. */
        void UnloadPlugin(DynLib* library);

    protected:
        /** @copydoc Module::OnStartUp */
        void OnStartUp() override;

        /** @copydoc Module::OnShutDown */
        void OnShutDown() override;

        /**	Called for each iteration of the main loop. Called before any game objects or plugins are updated. */
        virtual void PreUpdate() { }

        /**	Called for each iteration of the main loop. Called after all game objects and plugins are updated. */
        virtual void PostUpdate() { }

        /** Called after core initialization */
        virtual void PostStartUp() { }

        /** Call before core shutdown */
        virtual void PreShutDown() { }

    protected:
        typedef void(*UpdatePluginFunc)();

        SPtr<GuiAPI> _gui;
        SPtr<Renderer> _renderer;
        SPtr<RenderWindow> _window;
        START_UP_DESC _startUpDesc;

        // Frame limiting
        UINT64 _frameStep = 16666; // 60 times a second in microseconds
        UINT64 _lastFrameTime = 0; // Microseconds

        DynLib* _rendererPlugin;
        DynLib* _renderAPIPlugin;
        DynLib* _guiPlugin;
        Map<DynLib*, UpdatePluginFunc> _pluginUpdateFunctions;

        bool _isFrameRenderingFinished;

        volatile bool _runMainLoop;
        volatile bool _pause;

        ApplicationState _state;
    };

    /**	Provides easy access to CoreApplication. */
    TE_CORE_EXPORT CoreApplication& gCoreApplication();
    TE_CORE_EXPORT CoreApplication* gCoreApplicationPtr();
}
