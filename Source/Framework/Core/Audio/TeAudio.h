#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"
#include "Math/TeVector3.h"

namespace te
{
    /** Identifier for a device that can be used for playing audio. */
    struct TE_CORE_EXPORT AudioDevice
    {
        String Name;
    };

    /** Provides global functionality relating to sounds and music. */
    class TE_CORE_EXPORT Audio : public Module<Audio>
    {
    public:
        TE_MODULE_STATIC_HEADER_MEMBER(Audio)

        virtual ~Audio() = default;
    };

    /** Provides easier access to Audio. */
    TE_CORE_EXPORT Audio& gAudio();
}
