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
        ~NativeScript();

        /** 
         * Called when a new script using this native script is created
         */
        virtual void OnStartup() = 0;

        /**
         * Called when a script using this native script is destroyed (before)
         */
        virtual void OnShutdown() = 0;

        /**
         * Called when a script using this native script is disabled
         */
        virtual void OnDisabled() = 0;

        /**
         * Called when a script using this native script is enabled
         */
        virtual void OnEnabled() = 0;

        /**
         * Called before scene update
         */
        virtual void PreUpdate() = 0;

        /**
         * Called during scene update (sceneObject arbitrary order)
         */
        virtual void Update() = 0;

        /**
         * Called after scene update
         */
        virtual void PostUpdate() = 0;

        /**
         * Called after engine render
         */
        virtual void PostRender() = 0;
    };
}
