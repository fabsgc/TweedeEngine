#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"

namespace te
{
    /** Creates and destroys a specific physics implementation. */
    class TE_CORE_EXPORT PhysicsFactory
    {
    public:
        virtual ~PhysicsFactory() = default;

        /** Initializes the audio system. */
        virtual void StartUp() = 0;

        /** Shuts down the audio system. */
        virtual void ShutDown() = 0;
    };

    /** Takes care of loading, initializing and shutting down of a particular physics implementation. */
    class TE_CORE_EXPORT PhysicsManager : public Module<PhysicsManager>
    {
    public:
        TE_MODULE_STATIC_HEADER_MEMBER(PhysicsManager)

        /**
         * Initializes the physics manager and a particular physics implementation.
         *
         * @param[in]	pluginName	Name of the plugin containing a physics implementation.
         */
        PhysicsManager(const String& pluginName);
        virtual ~PhysicsManager();

    private:
        DynLib* _plugin = nullptr;
        PhysicsFactory* _factory = nullptr;
    };
}
