#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"

namespace te
{
    /** Creates and destroys a specific audio system implementation. */
    class TE_CORE_EXPORT AudioFactory
    {
    public:
        virtual ~AudioFactory() = default;

        /** Initializes the audio system. */
        virtual void StartUp() = 0;

        /** Shuts down the audio system. */
        virtual void ShutDown() = 0;
    };

    /** Takes care of loading, initializing and shutting down of a particular audio system implementation. */
    class TE_CORE_EXPORT AudioManager : public Module<AudioManager>
    {
    public:
        TE_MODULE_STATIC_HEADER_MEMBER(AudioManager)

        /**
         * Initializes the physics manager and a particular audio system implementation.
         *
         * @param[in]	pluginName	Name of the plugin containing a audio system implementation.
         */
        AudioManager(const String& pluginName);
        virtual ~AudioManager();

    private:
        DynLib* _plugin = nullptr;
        AudioFactory* _factory = nullptr;
    };
}
