#pragma once

#include "TeCorePrerequisites.h"
#include "Serialization/TeSerializable.h"

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
         * Returns library name used for this script
         */
        String GetLibraryName() { return _libraryName; }

    private:

        /** For file wathing, we need to know which dll is used behind each native script*/
        String _libraryName;
    };
}
