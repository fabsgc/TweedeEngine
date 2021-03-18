#pragma once

#include "TeCorePrerequisites.h"
#include "Serialization/TeSerializable.h"
#include "Scene/TeSceneObject.h"

// DLL export for plugins
#if TE_PLATFORM == TE_PLATFORM_WIN32 // Windows
#   if TE_COMPILER == TE_COMPILER_MSVC
#       define TE_SCRIPT_EXPORT __declspec(dllexport)
#   else
#       define TE_SCRIPT_EXPORT __attribute__ ((dllexport))
#   endif
#else // Linux/Mac settings
#   define TE_SCRIPT_EXPORT __attribute__ ((visibility ("default")))
#endif

namespace te
{
    /**
     * A native script is the base class of all scripts that can be written, loaded and executed inside the engine
     */
    class TE_CORE_EXPORT NativeScript : public Serializable
    {
    public:
        NativeScript();
        virtual ~NativeScript() = 0;

        /** 
         * Called when a new script using this native script is created
         */
        virtual void OnStartup() { }

        /**
         * Called when a script using this native script is destroyed (before)
         */
        virtual void OnShutdown() { }

        /**
         * Called when a script using this native script is disabled
         */
        virtual void OnDisabled() { }

        /**
         * Called when a script using this native script is enabled
         */
        virtual void OnEnabled() { }

        /**
         * Called before scene update
         */
        virtual void PreUpdate() { }

        /**
         * Called during scene update (sceneObject arbitrary order)
         */
        virtual void Update() { }

        /**
         * Called after scene update
         */
        virtual void PostUpdate() { }

        /**
         * Called after engine render
         */
        virtual void PostRender() { }

    public:
        /**
         * We want to internally keep the name of the lib used for this native script
         */
        void SetLibraryName(const String& name) { _libraryName = name; }

        /**
         * We want to internally keep the name of the lib used for this native script
         */
        void SetLibraryPath(const String& path) { _libraryPath = path; }

        /**
         * Returns library name used for this script
         */
        const String GetLibraryName() const { return _libraryName; }

        /**
         * Returns library path used for this script
         */
        const String GetLibraryPath() const { return _libraryPath; }

        /**
         * Because a script lives inside the scene graph we want to store an handle to its parent SceneObject
         */
        void SetParentSceneObject(const HSceneObject& sceneObject) 
        { 
            if (!sceneObject.Empty())
                _parentSO = sceneObject.GetNewHandleFromExisting();
            else
                _parentSO = HSceneObject();
        }

        /**
         * @copydoc SetParentSceneObject
         */
        const HSceneObject& GetParentSceneObject() const { return _parentSO; }

    private:
        /** For file wathing, we need to know which dll is used behind each native script */
        String _libraryName;
        String _libraryPath;
        HSceneObject _parentSO;
    };
}
